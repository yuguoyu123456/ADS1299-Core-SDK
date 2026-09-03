function result = eeg_basic_analysis(data, fs, mainsHz)
%EEG_BASIC_ANALYSIS Beginner EEG workflow for ADS1299-Core-SDK.
%
% Data convention: samples x channels, amplitudes in microvolts (uV).
% Requires MATLAB Signal Processing Toolbox for iirnotch, butter, filtfilt,
% and pwelch.
%
% Example with synthetic data:
%   result = eeg_basic_analysis();
%
% Example with real data:
%   result = eeg_basic_analysis(eeg_uV, 250, 50);
%
% This is research/education starter code, not a clinical processing standard.

if nargin < 1 || isempty(data)
    fs = 250;
    mainsHz = 50;
    duration = 20;
    t = (0:(fs*duration-1))' / fs;
    rng(7);
    data = zeros(length(t), 8);
    for ch = 1:8
        alphaHz = 9.5 + 0.2*(ch-1);
        data(:,ch) = ...
            (15 + ch).*sin(2*pi*alphaHz*t) + ...
            5.*sin(2*pi*6*t + 0.15*(ch-1)) + ...
            8.*sin(2*pi*mainsHz*t) + ...
            6.*randn(size(t));
    end
end

if nargin < 2 || isempty(fs)
    fs = 250;
end
if nargin < 3 || isempty(mainsHz)
    mainsHz = 50;
end

validateattributes(data, {'numeric'}, {'2d','nonempty','real'});
validateattributes(fs, {'numeric'}, {'scalar','positive'});
if mainsHz >= fs/2
    error('mainsHz must be below Nyquist.');
end

% 1) Remove linear trend channel-by-channel.
x = detrend(double(data), 'linear');

% 2) Common average reference (CAR).
if size(x,2) >= 2
    x = x - mean(x, 2, 'omitnan');
end

% 3) Zero-phase notch.
wo = mainsHz/(fs/2);
bw = wo/30; % approximate Q=30
[bn, an] = iirnotch(wo, bw);
x = filtfilt(bn, an, x);

% 4) Zero-phase 0.5-45 Hz Butterworth band-pass.
lowHz = 0.5;
highHz = min(45, fs/2 - 0.5);
[bpB, bpA] = butter(4, [lowHz highHz]/(fs/2), 'bandpass');
x = filtfilt(bpB, bpA, x);

% 5) Welch PSD.
nfftWindow = min(size(x,1), max(32, round(2*fs)));
noverlap = floor(nfftWindow/2);
for ch = 1:size(x,2)
    [p(:,ch), f] = pwelch(x(:,ch), hann(nfftWindow), noverlap, [], fs); %#ok<AGROW>
end

% 6) Canonical band powers.
bands = struct( ...
    'delta', [0.5 4], ...
    'theta', [4 8], ...
    'alpha', [8 13], ...
    'beta',  [13 30], ...
    'gamma', [30 min(45, fs/2-0.5)]);

names = fieldnames(bands);
totalMask = f >= 0.5 & f <= min(45, fs/2-0.5);
totalPower = trapz(f(totalMask), p(totalMask,:), 1);
absolute = struct();
relative = struct();
for k = 1:numel(names)
    name = names{k};
    range = bands.(name);
    mask = f >= range(1) & f < range(2);
    if nnz(mask) >= 2
        pow = trapz(f(mask), p(mask,:), 1);
    else
        pow = nan(1,size(x,2));
    end
    absolute.(name) = pow;
    relative.(name) = pow ./ totalPower;
end

% 7) Alpha peak per channel.
alphaMask = f >= 8 & f <= 13;
alphaFreqs = f(alphaMask);
alphaPsd = p(alphaMask,:);
[~, idx] = max(alphaPsd, [], 1);
alphaPeakHz = alphaFreqs(idx);

% 8) Time-domain features.
timeFeatures.mean = mean(x, 1, 'omitnan');
timeFeatures.std = std(x, 0, 1, 'omitnan');
timeFeatures.rms = sqrt(mean(x.^2, 1, 'omitnan'));
timeFeatures.peakToPeak = max(x, [], 1) - min(x, [], 1);

% 9) Hjorth parameters.
dx = diff(x,1,1);
ddx = diff(dx,1,1);
var0 = var(x,0,1);
var1 = var(dx,0,1);
var2 = var(ddx,0,1);
mobility = sqrt(var1 ./ var0);
mobilityDx = sqrt(var2 ./ var1);
complexity = mobilityDx ./ mobility;

hjorth.activity = var0;
hjorth.mobility = mobility;
hjorth.complexity = complexity;

result.fs = fs;
result.filtered_uV = x;
result.freq_Hz = f;
result.psd_uV2_per_Hz = p;
result.bandpower_absolute = absolute;
result.bandpower_relative = relative;
result.alphaPeak_Hz = alphaPeakHz;
result.timeFeatures = timeFeatures;
result.hjorth = hjorth;

fprintf('ADS1299-Core-SDK MATLAB EEG starter\n');
fprintf('Samples: %d, channels: %d, fs: %.1f Hz\n', size(x,1), size(x,2), fs);
fprintf('Alpha peak (Hz): ');
fprintf('%.2f ', alphaPeakHz);
fprintf('\n');
end
