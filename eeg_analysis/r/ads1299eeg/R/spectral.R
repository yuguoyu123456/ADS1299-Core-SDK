.hann <- function(n) {
  if (n < 2L) return(rep(1, n))
  0.5 - 0.5 * cos(2 * pi * (0:(n - 1L)) / (n - 1L))
}

.trapz <- function(x, y) {
  if (length(x) < 2L) return(NA_real_)
  sum(diff(x) * (head(y, -1L) + tail(y, -1L)) / 2)
}

#' Welch power spectral density estimate for samples x channels EEG
welch_psd <- function(x, fs, segment_seconds = 2, overlap = 0.5, nfft = NULL) {
  x <- validate_eeg(x, fs, min_samples = 8L)
  if (!is.finite(segment_seconds) || segment_seconds <= 0) stop("segment_seconds must be positive")
  if (!is.finite(overlap) || overlap < 0 || overlap >= 1) stop("overlap must satisfy 0 <= overlap < 1")

  nseg <- min(nrow(x), max(8L, as.integer(round(segment_seconds * fs))))
  if (is.null(nfft)) nfft <- nseg
  nfft <- as.integer(round(nfft))
  if (nfft < nseg) nfft <- nseg
  noverlap <- min(nseg - 1L, as.integer(floor(nseg * overlap)))
  step <- nseg - noverlap
  starts <- seq.int(1L, nrow(x) - nseg + 1L, by = step)
  if (!length(starts)) starts <- 1L

  w <- .hann(nseg)
  scale <- fs * sum(w^2)
  nfreq <- floor(nfft / 2L) + 1L
  accum <- matrix(0, nfreq, ncol(x))

  for (s in starts) {
    seg <- x[s:(s + nseg - 1L), , drop = FALSE]
    seg <- sweep(seg, 2L, colMeans(seg), "-")
    seg <- seg * w
    for (ch in seq_len(ncol(seg))) {
      z <- if (nfft > nseg) c(seg[, ch], rep(0, nfft - nseg)) else seg[, ch]
      y <- fft(z)
      p <- (Mod(y[seq_len(nfreq)])^2) / scale
      if ((nfft %% 2L) == 0L) {
        if (nfreq > 2L) p[2:(nfreq - 1L)] <- 2 * p[2:(nfreq - 1L)]
      } else if (nfreq > 1L) {
        p[2:nfreq] <- 2 * p[2:nfreq]
      }
      accum[, ch] <- accum[, ch] + p
    }
  }
  accum <- accum / length(starts)
  freq <- (0:(nfreq - 1L)) * fs / nfft
  colnames(accum) <- colnames(x)
  list(freq_hz = freq, psd = accum, n_segments = length(starts), segment_samples = nseg, nfft = nfft)
}

.default_bands <- function(fs, max_hz = 45) {
  upper <- min(max_hz, fs / 2 - max(0.01, fs * 1e-6))
  list(
    delta = c(0.5, min(4, upper)),
    theta = c(4, min(8, upper)),
    alpha = c(8, min(13, upper)),
    beta = c(13, min(30, upper)),
    gamma = c(30, upper)
  )
}

#' Absolute and relative canonical EEG band powers from a Welch result
eeg_band_powers <- function(psd_result, bands = NULL, total_range = NULL) {
  f <- psd_result$freq_hz
  p <- as.matrix(psd_result$psd)
  if (is.null(bands)) {
    fs_est <- if (length(f) > 1L) 2 * max(f) else 250
    bands <- .default_bands(fs_est)
  }
  if (is.null(total_range)) total_range <- c(0.5, max(vapply(bands, function(b) b[2], numeric(1))))

  integrate_band <- function(lo, hi) {
    idx <- which(f >= lo & f <= hi)
    if (length(idx) < 2L || hi <= lo) return(rep(NA_real_, ncol(p)))
    vapply(seq_len(ncol(p)), function(ch) .trapz(f[idx], p[idx, ch]), numeric(1))
  }

  absolute <- lapply(bands, function(b) integrate_band(b[1], b[2]))
  total <- integrate_band(total_range[1], total_range[2])
  relative <- lapply(absolute, function(v) v / total)
  list(absolute = absolute, relative = relative, total = total, bands = bands)
}

#' Dominant alpha frequency per channel
alpha_peak_frequency <- function(psd_result, range_hz = c(8, 13)) {
  f <- psd_result$freq_hz
  p <- as.matrix(psd_result$psd)
  idx <- which(f >= range_hz[1] & f <= range_hz[2])
  if (!length(idx)) return(rep(NA_real_, ncol(p)))
  vapply(seq_len(ncol(p)), function(ch) f[idx[which.max(p[idx, ch])]], numeric(1))
}

#' Normalized spectral entropy per channel
spectral_entropy <- function(psd_result, range_hz = c(0.5, 45)) {
  f <- psd_result$freq_hz
  p <- as.matrix(psd_result$psd)
  idx <- which(f >= range_hz[1] & f <= range_hz[2])
  if (length(idx) < 2L) return(rep(NA_real_, ncol(p)))
  vapply(seq_len(ncol(p)), function(ch) {
    q <- p[idx, ch]
    q <- q[is.finite(q) & q > 0]
    if (length(q) < 2L) return(NA_real_)
    q <- q / sum(q)
    -sum(q * log(q)) / log(length(q))
  }, numeric(1))
}

.line_noise_ratio <- function(psd_result, mains_hz = 50, line_half_width = 1, reference_half_width = 5) {
  f <- psd_result$freq_hz
  p <- as.matrix(psd_result$psd)
  line <- which(abs(f - mains_hz) <= line_half_width)
  ref <- which(abs(f - mains_hz) <= reference_half_width & abs(f - mains_hz) > line_half_width)
  if (length(line) < 1L || length(ref) < 2L) return(rep(NA_real_, ncol(p)))
  vapply(seq_len(ncol(p)), function(ch) mean(p[line, ch]) / max(mean(p[ref, ch]), .Machine$double.eps), numeric(1))
}
