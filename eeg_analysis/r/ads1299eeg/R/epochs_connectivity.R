#' Extract event-locked EEG epochs
#'
#' event_samples are one-based sample indices, matching R indexing.
extract_epochs <- function(x, fs, event_samples, tmin = -0.2, tmax = 0.8, drop_out_of_bounds = TRUE) {
  x <- validate_eeg(x, fs)
  if (!length(event_samples)) stop("event_samples is empty")
  event_samples <- as.integer(round(event_samples))
  if (!is.finite(tmin) || !is.finite(tmax) || tmax <= tmin) stop("require finite tmax > tmin")
  offsets <- seq.int(as.integer(round(tmin * fs)), as.integer(round(tmax * fs)))
  time_s <- offsets / fs

  accepted <- integer()
  blocks <- list()
  for (ev in event_samples) {
    idx <- ev + offsets
    if (min(idx) < 1L || max(idx) > nrow(x)) {
      if (drop_out_of_bounds) next
      stop(sprintf("event at sample %d exceeds signal bounds", ev))
    }
    accepted <- c(accepted, ev)
    blocks[[length(blocks) + 1L]] <- x[idx, , drop = FALSE]
  }
  if (!length(blocks)) stop("no events remain after boundary checks")

  out <- array(NA_real_, dim = c(length(offsets), ncol(x), length(blocks)))
  for (k in seq_along(blocks)) out[, , k] <- blocks[[k]]
  dimnames(out) <- list(NULL, colnames(x), paste0("event_", accepted))
  attr(out, "time_s") <- time_s
  attr(out, "event_samples") <- accepted
  attr(out, "fs") <- fs
  out
}

#' Baseline-correct event epochs
baseline_correct <- function(epochs, baseline = c(-0.2, 0), time_s = attr(epochs, "time_s")) {
  d <- dim(epochs)
  if (length(d) != 3L) stop("epochs must be time x channels x trials")
  if (is.null(time_s) || length(time_s) != d[1]) stop("time_s is missing or has wrong length")
  idx <- which(time_s >= baseline[1] & time_s <= baseline[2])
  if (!length(idx)) stop("baseline window contains no samples")
  out <- epochs
  for (k in seq_len(d[3])) {
    for (ch in seq_len(d[2])) {
      b <- mean(epochs[idx, ch, k], na.rm = TRUE)
      out[, ch, k] <- epochs[, ch, k] - b
    }
  }
  attributes(out) <- attributes(epochs)
  out
}

#' ERP mean or median and standard error across trials
erp_average <- function(epochs, method = c("mean", "median")) {
  method <- match.arg(method)
  d <- dim(epochs)
  if (length(d) != 3L) stop("epochs must be time x channels x trials")
  center_fun <- if (method == "mean") mean else stats::median
  center <- apply(epochs, c(1L, 2L), center_fun, na.rm = TRUE)
  sem <- if (d[3] > 1L) {
    apply(epochs, c(1L, 2L), stats::sd, na.rm = TRUE) / sqrt(d[3])
  } else {
    matrix(NA_real_, d[1], d[2])
  }
  list(
    time_s = attr(epochs, "time_s"),
    waveform = center,
    sem = sem,
    n_trials = d[3],
    method = method
  )
}

#' Band-limited magnitude-squared coherence matrix
coherence_matrix <- function(x, fs, band_hz = c(8, 13), segment_seconds = 2, overlap = 0.5) {
  x <- validate_eeg(x, fs, min_samples = 16L)
  if (length(band_hz) != 2L || band_hz[1] < 0 || band_hz[2] <= band_hz[1] || band_hz[2] >= fs / 2) {
    stop("band_hz must lie inside [0, Nyquist)")
  }
  nseg <- min(nrow(x), max(16L, as.integer(round(segment_seconds * fs))))
  noverlap <- min(nseg - 1L, as.integer(floor(nseg * overlap)))
  step <- nseg - noverlap
  starts <- seq.int(1L, nrow(x) - nseg + 1L, by = step)
  if (!length(starts)) starts <- 1L
  w <- .hann(nseg)
  nfreq <- floor(nseg / 2L) + 1L
  nch <- ncol(x)
  pxx <- matrix(0, nfreq, nch)
  pxy <- array(complex(real = 0, imaginary = 0), dim = c(nfreq, nch, nch))

  for (s in starts) {
    seg <- x[s:(s + nseg - 1L), , drop = FALSE]
    seg <- sweep(seg, 2L, colMeans(seg), "-")
    fmat <- matrix(complex(real = 0, imaginary = 0), nfreq, nch)
    for (ch in seq_len(nch)) {
      fmat[, ch] <- fft(seg[, ch] * w)[seq_len(nfreq)]
      pxx[, ch] <- pxx[, ch] + Mod(fmat[, ch])^2
    }
    for (i in seq_len(nch)) {
      for (j in i:nch) {
        cxy <- fmat[, i] * Conj(fmat[, j])
        pxy[, i, j] <- pxy[, i, j] + cxy
        if (j != i) pxy[, j, i] <- pxy[, j, i] + Conj(cxy)
      }
    }
  }
  pxx <- pxx / length(starts)
  pxy <- pxy / length(starts)
  freq <- (0:(nfreq - 1L)) * fs / nseg
  idx <- which(freq >= band_hz[1] & freq <= band_hz[2])
  if (!length(idx)) stop("requested coherence band has no FFT bins")

  out <- diag(1, nch)
  for (i in seq_len(nch)) {
    for (j in i:nch) {
      den <- pxx[, i] * pxx[, j]
      coh <- Mod(pxy[, i, j])^2 / pmax(den, .Machine$double.eps)
      value <- mean(pmin(1, pmax(0, coh[idx])), na.rm = TRUE)
      out[i, j] <- value
      out[j, i] <- value
    }
  }
  dimnames(out) <- list(colnames(x), colnames(x))
  out
}
