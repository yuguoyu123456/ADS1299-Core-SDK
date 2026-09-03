.odd_taps <- function(taps) {
  taps <- as.integer(round(taps))
  if (taps < 5L) stop("taps must be at least 5")
  if ((taps %% 2L) == 0L) taps <- taps + 1L
  taps
}

.sinc <- function(x) {
  y <- rep(1, length(x))
  nz <- abs(x) > .Machine$double.eps^0.5
  y[nz] <- sin(pi * x[nz]) / (pi * x[nz])
  y
}

.lowpass_kernel <- function(cutoff_hz, fs, taps) {
  taps <- .odd_taps(taps)
  if (!is.finite(cutoff_hz) || cutoff_hz <= 0 || cutoff_hz >= fs / 2) {
    stop("cutoff_hz must lie between 0 and Nyquist")
  }
  m <- (taps - 1L) / 2L
  n <- (-m):m
  fc <- cutoff_hz / fs
  h <- 2 * fc * .sinc(2 * fc * n)
  w <- 0.54 - 0.46 * cos(2 * pi * (0:(taps - 1L)) / (taps - 1L))
  h <- h * w
  h / sum(h)
}

#' Design a symmetric windowed-sinc FIR band-pass
fir_bandpass <- function(low_hz, high_hz, fs, taps = 251L) {
  taps <- .odd_taps(taps)
  if (!is.finite(low_hz) || !is.finite(high_hz) || low_hz <= 0 ||
      high_hz <= low_hz || high_hz >= fs / 2) {
    stop("require 0 < low_hz < high_hz < Nyquist")
  }
  .lowpass_kernel(high_hz, fs, taps) - .lowpass_kernel(low_hz, fs, taps)
}

#' Design a symmetric windowed-sinc FIR band-stop
fir_bandstop <- function(low_hz, high_hz, fs, taps = 251L) {
  bp <- fir_bandpass(low_hz, high_hz, fs, taps)
  h <- -bp
  h[(length(h) + 1L) / 2L] <- h[(length(h) + 1L) / 2L] + 1
  # Preserve DC gain exactly despite floating-point windowing error.
  h / sum(h)
}

.reflect_pad <- function(v, pad) {
  n <- length(v)
  if (pad < 1L) return(v)
  if (n < pad + 2L) stop("signal is too short for the selected FIR length")
  left <- rev(v[2:(pad + 1L)])
  right <- rev(v[(n - pad):(n - 1L)])
  c(left, v, right)
}

#' Apply a symmetric FIR with reflection padding and centered zero-phase alignment
#'
#' This is offline filtering. It is intentionally non-causal and should not be
#' used in a real-time feedback loop without replacing it with a causal design.
apply_fir_zero_phase <- function(x, h) {
  x <- as.matrix(x)
  h <- as.numeric(h)
  if (length(h) < 3L || (length(h) %% 2L) == 0L) stop("h must have odd length >= 3")
  if (any(!is.finite(h))) stop("h contains non-finite values")
  pad <- (length(h) - 1L) / 2L
  out <- matrix(NA_real_, nrow(x), ncol(x))
  colnames(out) <- colnames(x)
  for (ch in seq_len(ncol(x))) {
    p <- .reflect_pad(x[, ch], pad)
    y <- as.numeric(stats::filter(p, h, sides = 2L))
    keep <- (pad + 1L):(pad + nrow(x))
    out[, ch] <- y[keep]
  }
  out
}

#' Offline zero-phase FIR mains notch
notch_filter <- function(x, fs, mains_hz = 50, width_hz = 2, taps = 251L) {
  if (!is.finite(mains_hz) || !is.finite(width_hz) || width_hz <= 0) {
    stop("mains_hz and width_hz must be finite and width_hz positive")
  }
  lo <- mains_hz - width_hz / 2
  hi <- mains_hz + width_hz / 2
  if (lo <= 0 || hi >= fs / 2) stop("notch band must be inside (0, Nyquist)")
  apply_fir_zero_phase(x, fir_bandstop(lo, hi, fs, taps))
}

#' Offline zero-phase FIR EEG band-pass
bandpass_filter <- function(x, fs, low_hz = 0.5, high_hz = 45, taps = 251L) {
  high_hz <- min(high_hz, fs / 2 - max(0.01, fs * 1e-6))
  if (high_hz <= low_hz) stop("band-pass is empty at this sampling rate")
  apply_fir_zero_phase(x, fir_bandpass(low_hz, high_hz, fs, taps))
}
