# ads1299eeg for R

Reusable R signal-processing utilities for ADS1299 EEG data. The package keeps the same core conventions as the Python and MATLAB layers in ADS1299-Core-SDK:

- matrices are **samples x channels**;
- EEG amplitudes are **microvolts (uV)** after ADC conversion;
- event sample indices are **1-based** in R;
- offline filtering is explicitly marked as **zero-phase / non-causal**;
- engineering quality thresholds are configurable and are not clinical criteria.

## Install from the repository checkout

```r
install.packages("eeg_analysis/r/ads1299eeg", repos = NULL, type = "source")
library(ads1299eeg)
```

For iterative development:

```bash
R CMD INSTALL eeg_analysis/r/ads1299eeg
```

## ADS1299 conversion

```r
library(ads1299eeg)

bytes <- rbind(
  c(0x00, 0x00, 0x01),
  c(0xFF, 0xFF, 0xFF),
  c(0x80, 0x00, 0x00)
)

codes <- ads1299_decode24(bytes)
uv <- ads1299_codes_to_uv(codes, vref = 4.5, gain = 24)
```

## Complete EEG starter pipeline

```r
fs <- 250
t <- (0:(20 * fs - 1)) / fs
set.seed(7)

x <- cbind(
  Fp1 = 20 * sin(2*pi*10*t) + 5 * sin(2*pi*50*t) + rnorm(length(t), sd = 3),
  Fp2 = 18 * sin(2*pi*10.4*t + 0.3) + 5 * sin(2*pi*50*t) + rnorm(length(t), sd = 3),
  C3  = 10 * sin(2*pi*6*t) + rnorm(length(t), sd = 4),
  C4  = 10 * sin(2*pi*6.5*t) + rnorm(length(t), sd = 4)
)

result <- eeg_pipeline(x, fs = fs, mains_hz = 50,
                       low_hz = 0.5, high_hz = 45)

result$alpha_peak_hz
result$bandpower$relative$alpha
result$quality_raw
```

## ERP workflow

```r
epochs <- extract_epochs(x, fs, event_samples = c(500, 1000, 1500),
                         tmin = -0.2, tmax = 0.8)
epochs <- baseline_correct(epochs, baseline = c(-0.2, 0))
erp <- erp_average(epochs)
```

## Connectivity starter

```r
alpha_coherence <- coherence_matrix(x, fs, band_hz = c(8, 13))
```

## Scope

This package is intended for research, education, hardware validation and transparent BCI prototyping. It does not attempt to replace MNE, EEGLAB, FieldTrip or other mature neuroscience ecosystems. When interoperability with those ecosystems is added, conversions should preserve sampling rate, channel order, units, events and metadata explicitly.

The implementations in this directory are project-native. Published methods and mature open-source projects can be used to verify expected behavior; directly incorporated third-party source must keep its original license and attribution.
