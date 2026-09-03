function codes = decode24(bytes)
%DECODE24 Decode ADS1299 MSB-first 24-bit two's-complement samples.
%
% CODES = ads1299eeg.decode24(BYTES)
%
% BYTES may be:
%   - a 1x3 / 3x1 vector for one sample;
%   - an Nx3 matrix for N samples.
%
% The return value is an Nx1 double vector containing exact integer codes in
% the ADS1299 range -8388608 ... 8388607. Double precision represents every
% signed 24-bit integer exactly in MATLAB and GNU Octave.

if nargin ~= 1 || ~isnumeric(bytes) || isempty(bytes)
    error('ads1299eeg:decode24:InvalidInput', ...
        'Input must be a nonempty numeric byte array.');
end

if isvector(bytes)
    if numel(bytes) ~= 3
        error('ads1299eeg:decode24:ByteCount', ...
            'A single ADS1299 code requires exactly three bytes.');
    end
    bytes = reshape(bytes, 1, 3);
elseif size(bytes,2) ~= 3
    error('ads1299eeg:decode24:Shape', ...
        'Input must have three columns: MSB, middle byte, LSB.');
end

b = double(bytes);
if any(~isfinite(b(:))) || any(b(:) < 0) || any(b(:) > 255) || any(b(:) ~= floor(b(:)))
    error('ads1299eeg:decode24:ByteRange', ...
        'Every byte must be an integer in the range 0..255.');
end

unsigned = b(:,1) * 65536 + b(:,2) * 256 + b(:,3);
codes = unsigned;
negative = unsigned >= 8388608; % bit 23 set
codes(negative) = codes(negative) - 16777216;
end
