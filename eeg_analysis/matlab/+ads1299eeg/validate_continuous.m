function [x, wasVector] = validate_continuous(data)
%VALIDATE_CONTINUOUS Validate continuous EEG as samples-by-channels.
%
% [X, WASVECTOR] = ads1299eeg.validate_continuous(DATA) converts DATA to
% double precision and returns a samples-by-channels matrix. A vector is
% interpreted as one EEG channel.
%
% This helper intentionally rejects N-D arrays so that filtering and feature
% functions cannot silently operate along an unexpected dimension.

if nargin ~= 1
    error('ads1299eeg:validate_continuous:ArgumentCount', ...
        'Exactly one input argument is required.');
end
if ~isnumeric(data) || ~isreal(data) || isempty(data)
    error('ads1299eeg:validate_continuous:InvalidData', ...
        'EEG data must be a nonempty real numeric array.');
end

wasVector = isvector(data);
if wasVector
    x = double(data(:));
elseif ndims(data) == 2
    x = double(data);
else
    error('ads1299eeg:validate_continuous:Dimensions', ...
        'Continuous EEG must be a vector or samples-by-channels matrix.');
end

if size(x,1) < 2 || size(x,2) < 1
    error('ads1299eeg:validate_continuous:TooShort', ...
        'EEG data must contain at least two samples and one channel.');
end
end
