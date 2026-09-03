#' Validate an EEG matrix
#'
#' Data convention throughout the package is samples x channels.
#' Amplitudes are assumed to be microvolts unless a function states otherwise.
validate_eeg <- function(x, fs, min_samples = 8L, allow_na = FALSE) {
  if (!is.numeric(x)) stop("x must be numeric")
  x <- as.matrix(x)
  storage.mode(x) <- "double"
  if (length(dim(x)) != 2L || ncol(x) < 1L || nrow(x) < min_samples) {
    stop("x must be a samples x channels matrix with enough samples")
  }
  if (!is.numeric(fs) || length(fs) != 1L || !is.finite(fs) || fs <= 0) {
    stop("fs must be one positive finite number")
  }
  if (!allow_na && any(!is.finite(x))) stop("x contains NA/NaN/Inf")
  x
}

#' Fill finite internal dropouts by linear interpolation
interpolate_dropouts <- function(x) {
  x <- as.matrix(x)
  out <- x
  for (ch in seq_len(ncol(out))) {
    v <- out[, ch]
    ok <- is.finite(v)
    if (all(ok)) next
    if (sum(ok) < 2L) stop(sprintf("channel %d has fewer than two finite samples", ch))
    idx <- seq_along(v)
    out[, ch] <- stats::approx(idx[ok], v[ok], xout = idx, rule = 2)$y
  }
  out
}

#' Decode ADS1299 big-endian signed 24-bit samples
#'
#' bytes may be an n x 3 integer matrix or a vector whose length is a multiple
#' of three. Returned values are doubles because base R integers are 32-bit.
ads1299_decode24 <- function(bytes) {
  if (is.raw(bytes)) bytes <- as.integer(bytes)
  if (is.vector(bytes) && is.null(dim(bytes))) {
    if ((length(bytes) %% 3L) != 0L) stop("byte vector length must be a multiple of 3")
    bytes <- matrix(bytes, ncol = 3L, byrow = TRUE)
  }
  bytes <- as.matrix(bytes)
  if (ncol(bytes) != 3L) stop("bytes must have exactly three columns")
  if (any(!is.finite(bytes)) || any(bytes < 0) || any(bytes > 255) || any(bytes != floor(bytes))) {
    stop("bytes must contain integer values from 0 to 255")
  }
  u <- bytes[, 1] * 65536 + bytes[, 2] * 256 + bytes[, 3]
  neg <- u >= 2^23
  u[neg] <- u[neg] - 2^24
  as.numeric(u)
}

#' Convert ADS1299 ADC codes to input-referred microvolts
#'
#' Uses LSB = VREF / (gain * 2^23), which is the ideal bipolar 24-bit scale.
ads1299_codes_to_uv <- function(codes, vref = 4.5, gain = 24) {
  if (!is.numeric(codes)) stop("codes must be numeric")
  if (!is.finite(vref) || vref <= 0) stop("vref must be positive")
  if (!is.finite(gain) || gain <= 0) stop("gain must be positive")
  as.numeric(codes) * (vref / (gain * 2^23)) * 1e6
}

#' Convert input-referred microvolts back to ideal ADS1299 ADC codes
ads1299_uv_to_codes <- function(uv, vref = 4.5, gain = 24, clip = TRUE) {
  if (!is.numeric(uv)) stop("uv must be numeric")
  if (!is.finite(vref) || vref <= 0) stop("vref must be positive")
  if (!is.finite(gain) || gain <= 0) stop("gain must be positive")
  codes <- round(as.numeric(uv) / ((vref / (gain * 2^23)) * 1e6))
  if (clip) codes <- pmax(-2^23, pmin(2^23 - 1, codes))
  codes
}

#' Common-average rereference
common_average_reference <- function(x) {
  x <- as.matrix(x)
  if (ncol(x) < 2L) return(x)
  x - rowMeans(x)
}
