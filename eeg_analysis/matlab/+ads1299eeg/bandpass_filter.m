function output = bandpass_filter(data, fs, lowHz, highHz, order)
%BANDPASS_FILTER Zero-phase Butterworth EEG band-pass filter.
%
% OUTPUT = ads1299eeg.bandpass_filter(DATA, FS, LOWHZ, HIGHHZ, ORDER)
% defaults to 0.5-45 Hz, order 4. DATA uses samples-by-channels convention.
%
% Requires BUTTER and FILTFILT from MATLAB Signal Processing Toolbox or GNU
% Octave's signal package.

if nargin < 3 || isempty(lowHz), lowHz = 0.5; end
if nargin < 4 || isempty(highHz), highHz = 45; end
if nargin < 5 || isempty(order), order = 4; end

[x, wasVector] = ads1299eeg.validate_continuous(data);
if ~isscalar(fs) || ~isfinite(fs) || fs <= 0
    error('ads1299eeg:bandpass_filter:SamplingRate', 'FS must be positive and finite.');
end
if ~isscalar(lowHz) || ~isscalar(highHz) || ...
        ~isfinite(lowHz) || ~isfinite(highHz) || ...
        lowHz <= 0 || highHz <= lowHz || highHz >= fs/2
    error('ads1299eeg:bandpass_filter:Frequencies', ...
        'Require 0 < LOWHZ < HIGHHZ < Nyquist.');
end
if ~isscalar(order) || order < 1 || order ~= floor(order)
    error('ads1299eeg:bandpass_filter:Order', 'ORDER must be a positive integer.');
end
if any(~isfinite(x(:)))
    error('ads1299eeg:bandpass_filter:NonFinite', 'Filtering requires finite input samples.');
end

[b, a] = butter(order, [lowHz highHz] / (fs/2), 'bandpass');
try
    output = filtfilt(b, a, x);
catch err
    error('ads1299eeg:bandpass_filter:FilterFailure', ...
        'Zero-phase band-pass filtering failed. Signal may be too short: %s', err.message);
end
if wasVector
    output = output(:,1);
end
end
