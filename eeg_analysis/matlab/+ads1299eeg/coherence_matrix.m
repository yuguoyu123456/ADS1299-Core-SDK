function matrix = coherence_matrix(data, fs, band, nperseg)
%COHERENCE_MATRIX Average magnitude-squared coherence by channel pair.
%
% MATRIX = ads1299eeg.coherence_matrix(DATA, FS, BAND, NPERSEG)
% computes Welch-style cross spectra with identical segmentation/windowing for
% every channel pair and averages magnitude-squared coherence inside BAND.
% Default BAND is alpha [8 13] Hz.

if nargin < 3 || isempty(band), band = [8 13]; end
[x, ~] = ads1299eeg.validate_continuous(data);
if any(~isfinite(x(:)))
    error('ads1299eeg:coherence_matrix:NonFinite', 'Coherence requires finite input data.');
end
if ~isscalar(fs) || fs <= 0 || ~isfinite(fs)
    error('ads1299eeg:coherence_matrix:SamplingRate', 'FS must be positive and finite.');
end
if numel(band) ~= 2 || band(1) < 0 || band(2) <= band(1) || band(2) >= fs/2
    error('ads1299eeg:coherence_matrix:Band', 'Require 0 <= BAND(1) < BAND(2) < Nyquist.');
end
if nargin < 4 || isempty(nperseg)
    nperseg = min(size(x,1), max(64, round(2*fs)));
end
nperseg = min(floor(nperseg), size(x,1));
overlap = floor(nperseg/2);
step = nperseg - overlap;
starts = 1:step:(size(x,1)-nperseg+1);
if isempty(starts)
    error('ads1299eeg:coherence_matrix:TooShort', 'Signal shorter than requested segment.');
end

n = (0:nperseg-1)';
window = 0.5 - 0.5*cos(2*pi*n/nperseg);
windowEnergy = sum(window.^2);
nfft = nperseg;
nFreq = floor(nfft/2)+1;
f = (0:nFreq-1)'*(fs/nfft);
mask = f >= band(1) & f <= band(2);
if ~any(mask)
    error('ads1299eeg:coherence_matrix:Bins', 'BAND contains no spectral bins.');
end

channels = size(x,2);
Pxx = zeros(nFreq,channels);
Pxy = zeros(nFreq,channels,channels);

for startIndex = starts
    segment = x(startIndex:startIndex+nperseg-1,:);
    segment = segment - mean(segment,1);
    segment = segment .* window(:,ones(1,channels));
    spectrum = fft(segment,nfft,1);
    spectrum = spectrum(1:nFreq,:);
    scale = fs*windowEnergy;
    for i = 1:channels
        Pxx(:,i) = Pxx(:,i) + abs(spectrum(:,i)).^2/scale;
        for j = i+1:channels
            cross = spectrum(:,i).*conj(spectrum(:,j))/scale;
            Pxy(:,i,j) = Pxy(:,i,j) + cross;
        end
    end
end
Pxx = Pxx/numel(starts);
Pxy = Pxy/numel(starts);

matrix = eye(channels);
for i = 1:channels
    for j = i+1:channels
        denominator = Pxx(:,i).*Pxx(:,j);
        coherence = zeros(nFreq,1);
        valid = denominator > 0;
        coherence(valid) = abs(Pxy(valid,i,j)).^2 ./ denominator(valid);
        value = mean(coherence(mask));
        matrix(i,j) = value;
        matrix(j,i) = value;
    end
end
end
