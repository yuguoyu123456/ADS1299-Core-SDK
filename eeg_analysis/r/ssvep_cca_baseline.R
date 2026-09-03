# Educational SSVEP CCA baseline in R.
# EEG convention: samples x channels.

ssvep_cca_baseline <- function(eeg, fs, frequencies, harmonics = 3) {
  X <- scale(as.matrix(eeg), center = TRUE, scale = TRUE)
  X[!is.finite(X)] <- 0
  n <- nrow(X)
  t <- (0:(n-1))/fs

  score_one <- function(f0) {
    Y <- do.call(cbind, lapply(1:harmonics, function(h) {
      cbind(sin(2*pi*h*f0*t), cos(2*pi*h*f0*t))
    }))
    Y <- scale(Y, center = TRUE, scale = TRUE)
    Y[!is.finite(Y)] <- 0
    ridge <- 1e-8
    Cxx <- crossprod(X)/(n-1) + diag(ridge, ncol(X))
    Cyy <- crossprod(Y)/(n-1) + diag(ridge, ncol(Y))
    Cxy <- crossprod(X,Y)/(n-1)
    invsqrt <- function(C) {
      e <- eigen((C+t(C))/2, symmetric=TRUE)
      e$vectors %*% diag(1/sqrt(pmax(e$values,ridge))) %*% t(e$vectors)
    }
    M <- invsqrt(Cxx) %*% Cxy %*% invsqrt(Cyy)
    min(max(svd(M, nu=0, nv=0)$d[1],0),1)
  }

  scores <- sapply(frequencies, score_one)
  list(best_frequency = frequencies[which.max(scores)], scores = scores)
}
