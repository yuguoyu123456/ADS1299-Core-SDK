function corrected = baseline_correct(epochs, times, baseline)
%BASELINE_CORRECT Subtract per-epoch/channel baseline mean.
%
% CORRECTED = ads1299eeg.baseline_correct(EPOCHS, TIMES, BASELINE)
% expects EPOCHS as epochs-by-samples-by-channels, TIMES in seconds and
% BASELINE=[start end]. Default baseline is [-0.2 0].

if nargin < 3 || isempty(baseline), baseline = [-0.2 0]; end
if ~isnumeric(epochs) || ~isreal(epochs) || ndims(epochs) ~= 3 || isempty(epochs)
    error('ads1299eeg:baseline_correct:Epochs', ...
        'EPOCHS must be a nonempty epochs-by-samples-by-channels numeric array.');
end
times = double(times(:));
if numel(times) ~= size(epochs,2)
    error('ads1299eeg:baseline_correct:Times', 'TIMES length must match epoch sample count.');
end
if ~isnumeric(baseline) || numel(baseline) ~= 2 || baseline(2) < baseline(1)
    error('ads1299eeg:baseline_correct:Window', 'BASELINE must be [start end].');
end
mask = times >= baseline(1) & times <= baseline(2);
if ~any(mask)
    error('ads1299eeg:baseline_correct:NoSamples', 'Baseline window contains no epoch samples.');
end

corrected = double(epochs);
for e = 1:size(corrected,1)
    for ch = 1:size(corrected,3)
        values = corrected(e,mask,ch);
        values = values(isfinite(values));
        if isempty(values)
            base = NaN;
        else
            base = mean(values);
        end
        corrected(e,:,ch) = corrected(e,:,ch) - base;
    end
end
end
