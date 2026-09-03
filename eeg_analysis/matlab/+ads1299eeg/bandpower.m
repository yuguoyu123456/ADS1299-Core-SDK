function result = bandpower(data, fs, bands, totalRange, nperseg)
%BANDPOWER Compute absolute and relative EEG band powers from Welch PSD.
%
% RESULT = ads1299eeg.bandpower(DATA, FS) uses canonical starter bands:
%   delta 0.5-4 Hz
%   theta 4-8 Hz
%   alpha 8-13 Hz
%   beta  13-30 Hz
%   gamma 30-45 Hz (when permitted by Nyquist)
%
% RESULT contains frequencyHz, psd, absolute, relative and totalPower.

if nargin < 3 || isempty(bands)
    bands = struct('delta',[0.5 4], 'theta',[4 8], 'alpha',[8 13], ...
        'beta',[13 30], 'gamma',[30 45]);
end
if nargin < 4 || isempty(totalRange)
    totalRange = [0.5 min(45, fs/2 - eps)];
end
if nargin < 5
    nperseg = [];
end

[frequencyHz, psd] = ads1299eeg.welch_psd(data, fs, nperseg);
if ~isnumeric(totalRange) || numel(totalRange) ~= 2 || totalRange(2) <= totalRange(1)
    error('ads1299eeg:bandpower:TotalRange', 'TOTALRANGE must be [low high].');
end

totalMask = frequencyHz >= totalRange(1) & frequencyHz <= totalRange(2);
if nnz(totalMask) < 2
    error('ads1299eeg:bandpower:Bins', 'TOTALRANGE contains too few PSD bins.');
end
totalPower = trapz(frequencyHz(totalMask), psd(totalMask,:), 1);

names = fieldnames(bands);
absolute = struct();
relative = struct();
for k = 1:numel(names)
    name = names{k};
    limits = bands.(name);
    if ~isnumeric(limits) || numel(limits) ~= 2 || limits(2) <= limits(1)
        error('ads1299eeg:bandpower:BandDefinition', 'Band %s must be [low high].', name);
    end
    high = min(limits(2), fs/2 - eps);
    if high <= limits(1)
        power = nan(1, size(psd,2));
    else
        mask = frequencyHz >= limits(1) & frequencyHz < high;
        if nnz(mask) < 2
            power = nan(1, size(psd,2));
        else
            power = trapz(frequencyHz(mask), psd(mask,:), 1);
        end
    end
    absolute.(name) = power;
    rel = nan(size(power));
    valid = isfinite(totalPower) & totalPower > 0;
    rel(valid) = power(valid) ./ totalPower(valid);
    relative.(name) = rel;
end

result.frequencyHz = frequencyHz;
result.psd = psd;
result.absolute = absolute;
result.relative = relative;
result.totalPower = totalPower;
end
