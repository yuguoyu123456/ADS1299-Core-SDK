function output = notch_filter(data, fs, frequency, q, harmonics)
%NOTCH_FILTER Zero-phase notch filter for mains interference.
%
% OUTPUT = ads1299eeg.notch_filter(DATA, FS, FREQUENCY, Q, HARMONICS)
% applies a second-order notch at FREQUENCY and optional integer harmonics
% below Nyquist. Defaults: FREQUENCY=50 Hz, Q=30, HARMONICS=1.
%
% Requires FILTFILT from MATLAB Signal Processing Toolbox or GNU Octave's
% signal package. The notch coefficients are calculated explicitly here so
% the frequency/Q definition remains transparent.

if nargin < 3 || isempty(frequency), frequency = 50; end
if nargin < 4 || isempty(q), q = 30; end
if nargin < 5 || isempty(harmonics), harmonics = 1; end

[x, wasVector] = ads1299eeg.validate_continuous(data);
if ~isscalar(fs) || ~isfinite(fs) || fs <= 0
    error('ads1299eeg:notch_filter:SamplingRate', 'FS must be a positive finite scalar.');
end
if ~isscalar(frequency) || ~isfinite(frequency) || frequency <= 0
    error('ads1299eeg:notch_filter:Frequency', 'FREQUENCY must be positive and finite.');
end
if ~isscalar(q) || ~isfinite(q) || q <= 0
    error('ads1299eeg:notch_filter:Q', 'Q must be positive and finite.');
end
if ~isscalar(harmonics) || harmonics < 1 || harmonics ~= floor(harmonics)
    error('ads1299eeg:notch_filter:Harmonics', 'HARMONICS must be a positive integer.');
end
if any(~isfinite(x(:)))
    error('ads1299eeg:notch_filter:NonFinite', 'Filtering requires finite input samples.');
end

output = x;
for h = 1:harmonics
    f0 = frequency * h;
    if f0 >= fs/2
        break;
    end

    % Digital biquad notch. Pole radius is derived from bandwidth=f0/Q.
    w0 = 2*pi*f0/fs;
    bandwidth = f0/q;
    radius = exp(-pi*bandwidth/fs);
    b = [1, -2*cos(w0), 1];
    a = [1, -2*radius*cos(w0), radius^2];

    % Normalize DC gain to unity where well defined.
    numeratorDc = sum(b);
    denominatorDc = sum(a);
    if abs(numeratorDc) > eps
        b = b * (denominatorDc / numeratorDc);
    end

    try
        output = filtfilt(b, a, output);
    catch err
        error('ads1299eeg:notch_filter:FilterFailure', ...
            'Zero-phase notch filtering failed. Signal may be too short: %s', err.message);
    end
end

if wasVector
    output = output(:,1);
end
end
