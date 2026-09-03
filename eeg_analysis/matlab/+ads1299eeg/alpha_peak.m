function peakHz = alpha_peak(data, fs, alphaRange, nperseg)
%ALPHA_PEAK Estimate strongest alpha-band Welch PSD frequency per channel.
%
% PEAKHZ = ads1299eeg.alpha_peak(DATA, FS, ALPHARANGE, NPERSEG)
% defaults to 8-13 Hz.

if nargin < 3 || isempty(alphaRange), alphaRange = [8 13]; end
if nargin < 4, nperseg = []; end
if ~isnumeric(alphaRange) || numel(alphaRange) ~= 2 || ...
        alphaRange(1) < 0 || alphaRange(2) <= alphaRange(1)
    error('ads1299eeg:alpha_peak:Range', 'ALPHARANGE must be [low high].');
end

[f, p] = ads1299eeg.welch_psd(data, fs, nperseg);
mask = f >= alphaRange(1) & f <= alphaRange(2);
if ~any(mask)
    error('ads1299eeg:alpha_peak:Bins', 'No PSD bins fall inside ALPHARANGE.');
end
localF = f(mask);
localP = p(mask,:);
[~, index] = max(localP, [], 1);
peakHz = localF(index)';
end
