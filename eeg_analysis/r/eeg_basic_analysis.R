# ADS1299-Core-SDK: beginner EEG analysis in base R
# Data convention: samples x channels, amplitudes in microvolts (uV).
# This is research/education starter code, not a clinical processing standard.

ads1299_eeg_basic <- function(x, fs = 250, mains_hz = 50) {
  x <- as.matrix(x)
  if (nrow(x) < 10 || ncol(x) < 1) stop("x must be samples x channels")
  if (fs <= 0) stop("fs must be positive")

  # Demean each channel.
  x <- sweep(x, 2, colMeans(x, na.rm = TRUE), "-")

  # Common-average reference when at least two channels are present.
  if (ncol(x) >= 2) {
    car <- rowMeans(x, na.rm = TRUE)
    x <- x - car
  }

  # Simple FFT-based PSD helper. For publication work, use a validated Welch
  # implementation (e.g. signal package) and document all parameters.
  psd_one <- function(v) {
    n <- length(v)
    w <- 0.5 - 0.5 * cos(2*pi*(0:(n-1))/(n-1))
    y <- fft(v * w)
    k <- 0:floor(n/2)
    p <- (Mod(y[k + 1])^2) / (fs * sum(w^2))
    if (length(p) > 2) p[2:(length(p)-1)] <- 2*p[2:(length(p)-1)]
    list(freq = k*fs/n, psd = p)
  }

  psd <- lapply(seq_len(ncol(x)), function(ch) psd_one(x[, ch]))
  freq <- psd[[1]]$freq
  pmat <- do.call(cbind, lapply(psd, `[[`, "psd"))

  band_power <- function(lo, hi) {
    idx <- which(freq >= lo & freq < hi)
    if (length(idx) < 2) return(rep(NA_real_, ncol(x)))
    df <- mean(diff(freq[idx]))
    colSums(pmat[idx, , drop = FALSE]) * df
  }

  bands <- list(
    delta = c(0.5, 4), theta = c(4, 8), alpha = c(8, 13),
    beta = c(13, 30), gamma = c(30, min(45, fs/2 - 0.01))
  )
  abs_power <- lapply(bands, function(b) band_power(b[1], b[2]))
  names(abs_power) <- names(bands)

  alpha_idx <- which(freq >= 8 & freq <= 13)
  alpha_peak_hz <- sapply(seq_len(ncol(x)), function(ch) {
    if (!length(alpha_idx)) return(NA_real_)
    freq[alpha_idx[which.max(pmat[alpha_idx, ch])]]
  })

  list(
    fs = fs,
    referenced_uV = x,
    freq_Hz = freq,
    psd_uV2_per_Hz = pmat,
    bandpower_absolute = abs_power,
    alpha_peak_Hz = alpha_peak_hz,
    note = paste("mains_hz =", mains_hz, "is metadata only in this base-R starter; add a validated notch/filter before scientific use")
  )
}
