function microvolts = codes_to_uv(codes, vref, gain)
%CODES_TO_UV Convert signed ADS1299 ADC codes to input-referred microvolts.
%
% UV = ads1299eeg.codes_to_uv(CODES, VREF, GAIN)
%
% VREF is the ADS1299 reference voltage in volts and GAIN is the configured
% PGA gain for the channel. The conversion follows the ADS1299 transfer scale:
%
%   LSB = (2 * VREF / GAIN) / 2^24 volts
%
% Defaults are VREF=4.5 V and GAIN=24, but callers should pass the values that
% match the actual acquisition configuration instead of relying on defaults.

if nargin < 2 || isempty(vref)
    vref = 4.5;
end
if nargin < 3 || isempty(gain)
    gain = 24;
end

if ~isnumeric(codes) || ~isreal(codes)
    error('ads1299eeg:codes_to_uv:InvalidCodes', 'CODES must be real numeric data.');
end
if ~isscalar(vref) || ~isfinite(vref) || vref <= 0
    error('ads1299eeg:codes_to_uv:InvalidVref', 'VREF must be a positive finite scalar.');
end
if ~isscalar(gain) || ~isfinite(gain) || gain <= 0
    error('ads1299eeg:codes_to_uv:InvalidGain', 'GAIN must be a positive finite scalar.');
end
if any(codes(:) < -8388608) || any(codes(:) > 8388607)
    error('ads1299eeg:codes_to_uv:Range', ...
        'CODES contains values outside the signed 24-bit ADS1299 range.');
end

lsbVolts = (2 * double(vref) / double(gain)) / 2^24;
microvolts = double(codes) .* lsbVolts .* 1e6;
end
