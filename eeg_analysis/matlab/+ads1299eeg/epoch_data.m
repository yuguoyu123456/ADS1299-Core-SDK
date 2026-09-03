function [epochs, times, keptEvents] = epoch_data(data, eventSamples, fs, tmin, tmax)
%EPOCH_DATA Extract complete fixed-length event-locked EEG epochs.
%
% [EPOCHS, TIMES, KEPT] = ads1299eeg.epoch_data(DATA, EVENTS, FS, TMIN, TMAX)
%
% DATA is samples-by-channels. EVENTS are 1-based sample indices, matching
% MATLAB indexing. EPOCHS is epochs-by-samples-by-channels. Events whose full
% requested window would fall outside DATA are omitted and returned only when
% present in KEPT.

[x, ~] = ads1299eeg.validate_continuous(data);
if ~isscalar(fs) || ~isfinite(fs) || fs <= 0
    error('ads1299eeg:epoch_data:SamplingRate', 'FS must be positive and finite.');
end
if ~isnumeric(eventSamples) || ~isvector(eventSamples)
    error('ads1299eeg:epoch_data:Events', 'EVENTSAMPLES must be a numeric vector.');
end
if ~isscalar(tmin) || ~isscalar(tmax) || ~isfinite(tmin) || ~isfinite(tmax) || tmax <= tmin
    error('ads1299eeg:epoch_data:Window', 'Require finite TMAX > TMIN.');
end

startOffset = round(tmin * fs);
stopOffsetExclusive = round(tmax * fs);
nTimes = stopOffsetExclusive - startOffset;
times = (0:nTimes-1)'/fs + tmin;

eventSamples = double(eventSamples(:));
if any(eventSamples ~= floor(eventSamples))
    error('ads1299eeg:epoch_data:IntegerEvents', 'Event sample indices must be integers.');
end

valid = false(size(eventSamples));
for k = 1:numel(eventSamples)
    first = eventSamples(k) + startOffset;
    last = eventSamples(k) + stopOffsetExclusive - 1;
    valid(k) = first >= 1 && last <= size(x,1);
end
keptEvents = eventSamples(valid);

epochs = zeros(numel(keptEvents), nTimes, size(x,2));
for k = 1:numel(keptEvents)
    first = keptEvents(k) + startOffset;
    last = keptEvents(k) + stopOffsetExclusive - 1;
    epochs(k,:,:) = reshape(x(first:last,:), [1 nTimes size(x,2)]);
end
end
