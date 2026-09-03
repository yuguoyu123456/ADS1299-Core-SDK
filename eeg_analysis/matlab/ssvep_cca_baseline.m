function [bestFreq, scores] = ssvep_cca_baseline(eeg, fs, frequencies, harmonics)
%SSVEP_CCA_BASELINE Educational CCA-style SSVEP reference implementation.
% eeg: samples x channels. Uses whitening/SVD and does not require canoncorr.

if nargin < 4, harmonics = 3; end
validateattributes(eeg, {'numeric'}, {'2d','nonempty','real'});
validateattributes(fs, {'numeric'}, {'scalar','positive'});
frequencies = frequencies(:)';

X = double(eeg);
X = X - mean(X,1);
X = X ./ max(std(X,0,1), eps);

scores = zeros(size(frequencies));
for k = 1:numel(frequencies)
    f0 = frequencies(k);
    t = (0:size(X,1)-1)'/fs;
    Y = [];
    for h = 1:harmonics
        Y = [Y sin(2*pi*h*f0*t) cos(2*pi*h*f0*t)]; %#ok<AGROW>
    end
    Y = Y - mean(Y,1);
    Y = Y ./ max(std(Y,0,1), eps);
    scores(k) = largestCanonicalCorrelation(X,Y);
end

[~, idx] = max(scores);
bestFreq = frequencies(idx);
end

function rho = largestCanonicalCorrelation(X,Y)
n = min(size(X,1),size(Y,1));
X = X(1:n,:); Y = Y(1:n,:);
ridge = 1e-8;
Cxx = (X'*X)/(n-1) + ridge*eye(size(X,2));
Cyy = (Y'*Y)/(n-1) + ridge*eye(size(Y,2));
Cxy = (X'*Y)/(n-1);
Wx = invsqrtm(Cxx,ridge);
Wy = invsqrtm(Cyy,ridge);
s = svd(Wx*Cxy*Wy);
rho = min(max(s(1),0),1);
end

function W = invsqrtm(C,ridge)
[V,D] = eig((C+C')/2);
d = max(diag(D),ridge);
W = V*diag(1./sqrt(d))*V';
end
