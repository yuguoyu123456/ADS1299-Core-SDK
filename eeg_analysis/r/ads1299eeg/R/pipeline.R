.linear_detrend <- function(x) {
  x <- as.matrix(x)
  t <- seq_len(nrow(x))
  tc <- t - mean(t)
  den <- sum(tc^2)
  out <- x
  for (ch in seq_len(ncol(x))) {
    v <- x[, ch]
    slope <- sum(tc * (v - mean(v))) / den
    out[, ch] <- v - (mean(v) + slope * tc)
  }
  out
}

.fit_filter_taps <- function(taps, n_samples) {
  max_taps <- max(5L, 2L * n_samples - 5L)
  taps <- min(.odd_taps(taps), max_taps)
  if ((taps %% 2L) == 0L) taps <- taps - 1L
  max(5L, taps)
}

#' Complete ADS1299 EEG starter processing pipeline
#'
#' The pipeline is intentionally explicit rather than magical: interpolate
#' finite dropouts, detrend, optional CAR, offline FIR mains notch and band-pass,
#' then compute spectra, band powers, alpha peaks, entropy, Hjorth features and
#' channel-quality reports. For online BCI, use causal filters instead.
eeg_pipeline <- function(x, fs = 250, mains_hz = 50,
                         low_hz = 0.5, high_hz = 45,
                         rereference = TRUE,
                         filter_taps = 251L,
                         notch_width_hz = 2) {
  raw <- validate_eeg(x, fs, min_samples = 16L, allow_na = TRUE)
  raw_quality <- channel_quality(raw, fs, mains_hz = mains_hz)
  filled <- interpolate_dropouts(raw)
  detrended <- .linear_detrend(filled)
  referenced <- if (rereference) common_average_reference(detrended) else detrended

  taps <- .fit_filter_taps(filter_taps, nrow(referenced))
  filtered <- referenced
  if (!is.null(mains_hz) && is.finite(mains_hz) && mains_hz > 0 && mains_hz < fs / 2) {
    filtered <- notch_filter(filtered, fs, mains_hz = mains_hz,
                             width_hz = notch_width_hz, taps = taps)
  }
  filtered <- bandpass_filter(filtered, fs, low_hz = low_hz,
                              high_hz = high_hz, taps = taps)

  psd <- welch_psd(filtered, fs)
  bands <- eeg_band_powers(psd)
  alpha <- alpha_peak_frequency(psd)
  entropy <- spectral_entropy(psd, c(max(0.5, low_hz), min(high_hz, fs / 2 - 0.01)))
  hjorth <- hjorth_parameters(filtered)
  processed_quality <- channel_quality(filtered, fs, mains_hz = mains_hz)

  time_features <- data.frame(
    channel = if (is.null(colnames(filtered))) seq_len(ncol(filtered)) else colnames(filtered),
    mean_uv = colMeans(filtered),
    sd_uv = apply(filtered, 2L, stats::sd),
    rms_uv = sqrt(colMeans(filtered^2)),
    peak_to_peak_uv = apply(filtered, 2L, function(v) diff(range(v))),
    alpha_peak_hz = alpha,
    spectral_entropy = entropy,
    row.names = NULL,
    check.names = FALSE
  )

  list(
    fs = fs,
    raw_uv = raw,
    filled_uv = filled,
    referenced_uv = referenced,
    filtered_uv = filtered,
    psd = psd,
    bandpower = bands,
    alpha_peak_hz = alpha,
    spectral_entropy = entropy,
    hjorth = hjorth,
    time_features = time_features,
    quality_raw = raw_quality,
    quality_processed = processed_quality,
    settings = list(
      mains_hz = mains_hz,
      low_hz = low_hz,
      high_hz = high_hz,
      rereference = rereference,
      filter_taps = taps,
      notch_width_hz = notch_width_hz,
      offline_zero_phase = TRUE
    )
  )
}
