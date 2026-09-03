# Educational P300/ERP baseline in R.
# epochs: trials x samples x channels; times in seconds.

p300_erp_baseline <- function(epochs, times, baseline=c(-0.2,0), p300_window=c(0.25,0.50)) {
  d <- dim(epochs)
  if (length(d) != 3) stop("epochs must be trials x samples x channels")
  b <- which(times >= baseline[1] & times <= baseline[2])
  if (!length(b)) stop("baseline has no samples")
  corrected <- epochs
  for (tr in seq_len(d[1])) for (ch in seq_len(d[3])) {
    corrected[tr,,ch] <- epochs[tr,,ch] - mean(epochs[tr,b,ch], na.rm=TRUE)
  }
  erp <- apply(corrected, c(2,3), mean, na.rm=TRUE)
  p <- which(times >= p300_window[1] & times <= p300_window[2])
  if (!length(p)) stop("P300 window has no samples")
  amp <- colMeans(erp[p,,drop=FALSE], na.rm=TRUE)
  latency <- sapply(seq_len(d[3]), function(ch) times[p[which.max(erp[p,ch])]])
  list(corrected_epochs=corrected, erp=erp, p300_mean_amplitude=amp, p300_peak_latency_s=latency)
}
