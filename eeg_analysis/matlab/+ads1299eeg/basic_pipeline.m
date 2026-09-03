function result = basic_pipeline(data, fs, options)
%BASIC_PIPELINE Transparent offline starter EEG analysis pipeline.
%
% RESULT = ads1299eeg.basic_pipeline(DATA, FS, OPTIONS)
%
% Processing order:
%   1) quality report on original data
%   2) remove per-channel mean
%   3) mains notch (if below Nyquist)
%   4) Butterworth band-pass
%   5) optional common-average reference
%   6) post-processing quality report
%   7) Welch band power, alpha peak and Hjorth features
%
% OPTIONS fields:
%   mainsHz       default 50
%   notchQ        default 30
%   harmonics     default 1
%   lowHz         default 0.5
%   highHz        default min(45, fs/2-0.5)
%   filterOrder   default 4
%   applyCAR      default false
%   nperseg       default []
%
% This is a documented baseline workflow, not a universal scientific or
% clinical preprocessing standard.

if nargin < 3 || isempty(options), options = struct(); end
options = set_default(options,'mainsHz',50);
options = set_default(options,'notchQ',30);
options = set_default(options,'harmonics',1);
options = set_default(options,'lowHz',0.5);
options = set_default(options,'highHz',min(45,fs/2-0.5));
options = set_default(options,'filterOrder',4);
options = set_default(options,'applyCAR',false);
options = set_default(options,'nperseg',[]);

[x, wasVector] = ads1299eeg.validate_continuous(data);
qualityBefore = ads1299eeg.quality_report(x, fs, options.mainsHz);

processed = x - mean(x,1);
if options.mainsHz < fs/2
    processed = ads1299eeg.notch_filter(processed, fs, ...
        options.mainsHz, options.notchQ, options.harmonics);
end
processed = ads1299eeg.bandpass_filter(processed, fs, ...
    options.lowHz, options.highHz, options.filterOrder);
if options.applyCAR
    processed = ads1299eeg.common_average_reference(processed);
end

qualityAfter = ads1299eeg.quality_report(processed, fs, options.mainsHz);
power = ads1299eeg.bandpower(processed, fs, [], [], options.nperseg);
alphaPeakHz = ads1299eeg.alpha_peak(processed, fs, [8 13], options.nperseg);
hjorthFeatures = ads1299eeg.hjorth(processed);

result.fs = fs;
result.raw = x;
result.processed = processed;
result.qualityBefore = qualityBefore;
result.qualityAfter = qualityAfter;
result.bandpower = power;
result.alphaPeakHz = alphaPeakHz;
result.hjorth = hjorthFeatures;
result.options = options;
result.inputWasVector = wasVector;
end

function options = set_default(options, field, value)
if ~isfield(options,field) || isempty(options.(field))
    options.(field) = value;
end
end
