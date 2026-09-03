function output = common_average_reference(data)
%COMMON_AVERAGE_REFERENCE Apply common-average reference (CAR).
%
% OUTPUT = ads1299eeg.common_average_reference(DATA) subtracts the
% instantaneous average across channels from every channel. DATA is
% samples-by-channels and must contain at least two channels.

[x, wasVector] = ads1299eeg.validate_continuous(data);
if wasVector || size(x,2) < 2
    error('ads1299eeg:common_average_reference:Channels', ...
        'Common-average reference requires at least two channels.');
end
if any(~isfinite(x(:)))
    error('ads1299eeg:common_average_reference:NonFinite', ...
        'CAR requires finite input samples.');
end

reference = mean(x, 2);
output = x - reference(:, ones(1, size(x,2)));
end
