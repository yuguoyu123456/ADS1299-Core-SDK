function [frequencyHz, psd] = welch_psd(data, fs, nperseg, noverlap)
%WELCH_PSD Estimate channel PSD with an explicit Welch implementation.
%
% [F, PSD] = ads1299eeg.welch_psd(DATA, FS, NPERSEG, NOVERLAP)
% returns F as frequency-by-1 and PSD as frequency-by-channels in input-unit^2/Hz.
%
% This implementation is intentionally self-contained rather than wrapping
% PWelch so MATLAB and GNU Octave use the same overlap/window semantics.

[x, ~] = ads1299eeg.validate_continuous(data);
if ~isscalar(fs) || ~isfinite(fs) || fs <= 0
    error('ads1299eeg:welch_psd:SamplingRate', 'FS must be a positive finite scalar.');
end
if any(~isfinite(x(:)))
    error('ads1299eeg:welch_psd:NonFinite', 'Welch PSD requires finite input data.');
end

if nargin < 3 || isempty(nperseg)
    nperseg = min(size(x,1), max(64, round(2*fs)));
end
nperseg = min(floor(double(nperseg)), size(x,1));
if nperseg < 8
    error('ads1299eeg:welch_psd:SegmentLength', 'NPERSEG must be at least 8 samples.');
end
if nargin < 4 || isempty(noverlap)
    noverlap = floor(nperseg/2);
end
noverlap = floor(double(noverlap));
if noverlap < 0 || noverlap >= nperseg
    error('ads1299eeg:welch_psd:Overlap', 'Require 0 <= NOVERLAP < NPERSEG.');
end

step = nperseg - noverlap;
starts = 1:step:(size(x,1)-nperseg+1);
if isempty(starts)
    error('ads1299eeg:welch_psd:TooShort', 'Signal is shorter than NPERSEG.');
end

% Periodic Hann window. Defining it here keeps output reproducible across
% MATLAB/Octave versions and avoids a toolbox dependency for spectral work.
n = (0:nperseg-1)';
window = 0.5 - 0.5*cos(2*pi*n/nperseg);
windowEnergy = sum(window.^2);
nfft = nperseg;
nFreq = floor(nfft/2) + 1;
psd = zeros(nFreq, size(x,2));

for startIndex = starts
    segment = x(startIndex:startIndex+nperseg-1, :);
    segment = segment - mean(segment, 1);
    segment = segment .* window(:, ones(1,size(x,2)));
    spectrum = fft(segment, nfft, 1);
    powerTwoSided = abs(spectrum).^2 / (fs * windowEnergy);
    powerOneSided = powerTwoSided(1:nFreq, :);

    if rem(nfft,2) == 0
        if nFreq > 2
            powerOneSided(2:end-1,:) = 2 * powerOneSided(2:end-1,:);
        end
    else
        if nFreq > 1
            powerOneSided(2:end,:) = 2 * powerOneSided(2:end,:);
        end
    end
    psd = psd + powerOneSided;
end

psd = psd / numel(starts);
frequencyHz = (0:nFreq-1)' * (fs/nfft);
end
