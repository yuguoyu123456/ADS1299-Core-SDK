# ADS1299-Core-SDK compatibility entry point.
#
# The implementation now lives in the installable ads1299eeg package under
# eeg_analysis/r/ads1299eeg. Keeping this wrapper avoids silently breaking
# users who previously sourced eeg_basic_analysis.R.

ads1299_eeg_basic <- function(x, fs = 250, mains_hz = 50,
                              low_hz = 0.5, high_hz = 45,
                              rereference = TRUE) {
  if (!requireNamespace("ads1299eeg", quietly = TRUE)) {
    stop(
      paste0(
        "The reusable ads1299eeg R package is not installed. From the repository root run: ",
        "install.packages('eeg_analysis/r/ads1299eeg', repos=NULL, type='source')"
      )
    )
  }
  ads1299eeg::eeg_pipeline(
    x = x,
    fs = fs,
    mains_hz = mains_hz,
    low_hz = low_hz,
    high_hz = high_hz,
    rereference = rereference
  )
}
