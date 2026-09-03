cat("ADS1299 R EEG numerical regression tests\n")

near <- function(x, y, tol) isTRUE(all(abs(x - y) <= tol))
nearest_bin <- function(freq, hz) which.min(abs(freq - hz))

# 1) Signed 24-bit decode.
bytes <- rbind(
  c(0, 0, 0),
  c(0, 0, 1),
  c(127, 255, 255),
  c(255, 255, 255),
  c(128, 0, 0)
)
stopifnot(identical(ads1299_decode24(bytes), c(0, 1, 8388607, -1, -8388608)))

# 2) ADS1299 ideal LSB / round trip.
lsb_uv <- (4.5 / (24 * 2^23)) * 1e6
stopifnot(near(ads1299_codes_to_uv(1, 4.5, 24), lsb_uv, 1e-12))
round_trip_codes <- c(-500000, -1, 0, 1, 500000)
stopifnot(identical(ads1299_uv_to_codes(ads1299_codes_to_uv(round_trip_codes)), round_trip_codes))

# Synthetic EEG used by several tests.
fs <- 250
n <- 20 * fs
t <- (0:(n - 1)) / fs
set.seed(42)
alpha <- 20 * sin(2*pi*10*t)
line <- 12 * sin(2*pi*50*t)
noise <- rnorm(n, sd = 2)
x1 <- alpha + line + noise
x2 <- 18 * sin(2*pi*10*t + 0.5) + line + rnorm(n, sd = 2)
x3 <- 12 * sin(2*pi*6*t) + rnorm(n, sd = 3)
x <- cbind(C1 = x1, C2 = x2, C3 = x3)

# 3) FIR notch must materially attenuate 50 Hz while retaining alpha.
ps_before <- welch_psd(x[, 1, drop = FALSE], fs)
x_notch <- notch_filter(x[, 1, drop = FALSE], fs, mains_hz = 50, width_hz = 2, taps = 251)
ps_after <- welch_psd(x_notch, fs)
i50 <- nearest_bin(ps_before$freq_hz, 50)
i10 <- nearest_bin(ps_before$freq_hz, 10)
atten_50 <- ps_after$psd[i50, 1] / ps_before$psd[i50, 1]
retain_10 <- ps_after$psd[i10, 1] / ps_before$psd[i10, 1]
stopifnot(is.finite(atten_50), atten_50 < 0.20)
stopifnot(is.finite(retain_10), retain_10 > 0.50)

# 4) Alpha peak and band powers.
filtered <- bandpass_filter(x_notch, fs, 0.5, 45, taps = 251)
ps <- welch_psd(filtered, fs)
peak <- alpha_peak_frequency(ps)
stopifnot(abs(peak[1] - 10) <= 0.75)
bp <- eeg_band_powers(ps)
stopifnot(bp$absolute$alpha[1] > bp$absolute$theta[1])
stopifnot(bp$absolute$alpha[1] > bp$absolute$beta[1])
stopifnot(bp$relative$alpha[1] > 0.50)

# 5) Spectral entropy is bounded.
ent <- spectral_entropy(ps)
stopifnot(all(is.finite(ent)), all(ent >= 0), all(ent <= 1 + 1e-12))

# 6) Quality detection: test flatline and line noise independently.
clean_good <- alpha + noise
q_flat <- channel_quality(
  cbind(good = clean_good, flat = rep(0, n)), fs,
  max_line_noise_ratio = Inf
)
stopifnot(!q_flat$bad[1])
stopifnot(q_flat$bad[2])
stopifnot(grepl("flat", q_flat$reason[2]))

q_line <- channel_quality(cbind(line_contaminated = x1), fs, max_line_noise_ratio = 5)
stopifnot(q_line$bad[1])
stopifnot(grepl("line_noise", q_line$reason[1]))

# 7) Hjorth metrics have expected dimensions and finite positive activity.
hj <- hjorth_parameters(x)
stopifnot(nrow(hj) == 3, all(is.finite(hj$activity)), all(hj$activity > 0))

# 8) Dropout interpolation.
x_drop <- x[, 1:2]
x_drop[100:110, 1] <- NA_real_
filled <- interpolate_dropouts(x_drop)
stopifnot(all(is.finite(filled)), nrow(filled) == n)

# 9) Epoch extraction and baseline correction.
events <- c(1000, 2000, 3000, 4000)
ep <- extract_epochs(x[, 1:2], fs, events, tmin = -0.2, tmax = 0.8)
stopifnot(identical(dim(ep), c(251L, 2L, 4L)))
ep0 <- baseline_correct(ep, baseline = c(-0.2, 0))
time_s <- attr(ep0, "time_s")
bidx <- which(time_s >= -0.2 & time_s <= 0)
for (k in seq_len(dim(ep0)[3])) {
  stopifnot(max(abs(colMeans(ep0[bidx, , k, drop = FALSE][, , 1]))) < 1e-8)
}
erp <- erp_average(ep0)
stopifnot(identical(dim(erp$waveform), c(251L, 2L)), erp$n_trials == 4L)

# 10) Coherence: validate the narrow band containing the known 10-Hz source.
# A wide 8-13 Hz average would also include noise-only bins, so it is not a
# valid test for the strength of one deliberately injected 10-Hz oscillator.
coh <- coherence_matrix(x[, 1:2], fs, band_hz = c(9.5, 10.5))
stopifnot(near(diag(coh), c(1, 1), 1e-12))
stopifnot(near(coh[1, 2], coh[2, 1], 1e-12))
stopifnot(coh[1, 2] > 0.70, all(coh >= 0), all(coh <= 1 + 1e-12))

# 11) Complete pipeline executes and returns internally consistent products.
result <- eeg_pipeline(x, fs = fs, mains_hz = 50, low_hz = 0.5, high_hz = 45)
stopifnot(identical(dim(result$filtered_uv), dim(x)))
stopifnot(length(result$alpha_peak_hz) == ncol(x))
stopifnot(nrow(result$time_features) == ncol(x))
stopifnot(nrow(result$quality_raw) == ncol(x))
stopifnot(all(is.finite(result$filtered_uv)))
stopifnot(abs(result$alpha_peak_hz[1] - 10) <= 0.75)

cat("All ADS1299 R EEG tests passed.\n")
