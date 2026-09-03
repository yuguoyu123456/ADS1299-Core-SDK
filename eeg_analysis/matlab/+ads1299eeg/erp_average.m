function result = erp_average(epochs, robust)
%ERP_AVERAGE Compute ERP waveform and standard error across epochs.
%
% RESULT = ads1299eeg.erp_average(EPOCHS, ROBUST)
% returns:
%   result.waveform  samples-by-channels mean (or median if ROBUST=true)
%   result.sem       samples-by-channels standard error of the mean
%   result.nEpochs   number of input epochs
%
% EPOCHS uses epochs-by-samples-by-channels convention.

if nargin < 2 || isempty(robust), robust = false; end
if ~isnumeric(epochs) || ~isreal(epochs) || ndims(epochs) ~= 3 || isempty(epochs)
    error('ads1299eeg:erp_average:Epochs', ...
        'EPOCHS must be epochs-by-samples-by-channels.');
end
if ~(isscalar(robust) && (islogical(robust) || isnumeric(robust)))
    error('ads1299eeg:erp_average:Robust', 'ROBUST must be scalar true/false.');
end

x = double(epochs);
nEpochs = size(x,1);
nTimes = size(x,2);
nChannels = size(x,3);
waveform = nan(nTimes,nChannels);
sem = nan(nTimes,nChannels);

for t = 1:nTimes
    for ch = 1:nChannels
        values = x(:,t,ch);
        values = values(isfinite(values));
        if isempty(values)
            continue;
        end
        if robust
            sorted = sort(values);
            n = numel(sorted);
            if rem(n,2) == 1
                waveform(t,ch) = sorted((n+1)/2);
            else
                waveform(t,ch) = mean(sorted(n/2:n/2+1));
            end
        else
            waveform(t,ch) = mean(values);
        end
        if numel(values) > 1
            sem(t,ch) = std(values,0) / sqrt(numel(values));
        end
    end
end

result.waveform = waveform;
result.sem = sem;
result.nEpochs = nEpochs;
end
