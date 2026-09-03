#' Hjorth activity, mobility and complexity
hjorth_parameters <- function(x) {
  x <- as.matrix(x)
  if (nrow(x) < 3L) stop("need at least three samples")
  activity <- apply(x, 2L, stats::var)
  dx <- apply(x, 2L, diff)
  ddx <- apply(dx, 2L, diff)
  if (is.vector(dx)) dx <- matrix(dx, ncol = 1L)
  if (is.vector(ddx)) ddx <- matrix(ddx, ncol = 1L)
  var1 <- apply(dx, 2L, stats::var)
  var2 <- apply(ddx, 2L, stats::var)
  mobility <- sqrt(var1 / activity)
  mobility_dx <- sqrt(var2 / var1)
  complexity <- mobility_dx / mobility
  data.frame(
    channel = seq_len(ncol(x)),
    activity = activity,
    mobility = mobility,
    complexity = complexity,
    row.names = NULL
  )
}

#' Channel-level engineering quality report
#'
#' Thresholds are intentionally explicit and configurable. They are starter
#' engineering heuristics, not universal physiological acceptance criteria.
channel_quality <- function(x, fs, mains_hz = 50,
                            min_sd_uv = 0.05,
                            max_peak_to_peak_uv = 1000,
                            max_flatline_fraction = 0.98,
                            flat_step_uv = 1e-3,
                            max_line_noise_ratio = 20) {
  x <- validate_eeg(x, fs, allow_na = TRUE)
  n_ch <- ncol(x)
  finite_fraction <- colMeans(is.finite(x))
  sd_uv <- rep(NA_real_, n_ch)
  rms_uv <- rep(NA_real_, n_ch)
  peak_to_peak_uv <- rep(NA_real_, n_ch)
  flatline_fraction <- rep(NA_real_, n_ch)

  for (ch in seq_len(n_ch)) {
    v <- x[, ch]
    v <- v[is.finite(v)]
    if (length(v) >= 2L) {
      sd_uv[ch] <- stats::sd(v)
      rms_uv[ch] <- sqrt(mean(v^2))
      peak_to_peak_uv[ch] <- diff(range(v))
      flatline_fraction[ch] <- mean(abs(diff(v)) <= flat_step_uv)
    }
  }

  line_ratio <- rep(NA_real_, n_ch)
  can_psd <- finite_fraction == 1 & nrow(x) >= max(16L, as.integer(round(fs)))
  if (all(can_psd) && mains_hz < fs / 2) {
    ps <- welch_psd(x, fs)
    line_ratio <- .line_noise_ratio(ps, mains_hz = mains_hz)
  }

  bad <- rep(FALSE, n_ch)
  reasons <- vector("character", n_ch)
  for (ch in seq_len(n_ch)) {
    r <- character()
    if (finite_fraction[ch] < 1) r <- c(r, "non_finite")
    if (is.finite(sd_uv[ch]) && sd_uv[ch] < min_sd_uv) r <- c(r, "near_flat")
    if (is.finite(peak_to_peak_uv[ch]) && peak_to_peak_uv[ch] > max_peak_to_peak_uv) r <- c(r, "large_amplitude")
    if (is.finite(flatline_fraction[ch]) && flatline_fraction[ch] > max_flatline_fraction) r <- c(r, "flatline")
    if (is.finite(line_ratio[ch]) && line_ratio[ch] > max_line_noise_ratio) r <- c(r, "line_noise")
    bad[ch] <- length(r) > 0L
    reasons[ch] <- if (length(r)) paste(r, collapse = ";") else "ok"
  }

  data.frame(
    channel = if (is.null(colnames(x))) seq_len(n_ch) else colnames(x),
    finite_fraction = finite_fraction,
    sd_uv = sd_uv,
    rms_uv = rms_uv,
    peak_to_peak_uv = peak_to_peak_uv,
    flatline_fraction = flatline_fraction,
    line_noise_ratio = line_ratio,
    bad = bad,
    reason = reasons,
    row.names = NULL,
    check.names = FALSE
  )
}
