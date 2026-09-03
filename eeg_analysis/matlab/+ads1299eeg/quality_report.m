function report = quality_report(data, fs, mainsHz, options)
%QUALITY_REPORT Generate transparent channel-wise EEG quality metrics.
%
% REPORT = ads1299eeg.quality_report(DATA, FS, MAINSHZ, OPTIONS)
% returns a struct with one channel record per input column. This is an
% engineering/research screen, not a clinical quality classifier.
%
% OPTIONS fields (all optional):
%   flatlineToleranceUv      default 0.02
%   maxFlatlineFraction      default 0.20
%   clippingUv               default 5000
%   maxClippingFraction      default 0.001
%   maxLineNoiseRatio        default 0.25
%   maxPeakToPeakUv          default 2000
%   minStdUv                 default 0.05

if nargin < 3 || isempty(mainsHz), mainsHz = 50; end
if nargin < 4 || isempty(options), options = struct(); end
[x, ~] = ads1299eeg.validate_continuous(data);
if ~isscalar(fs) || ~isfinite(fs) || fs <= 0
    error('ads1299eeg:quality_report:SamplingRate', 'FS must be positive and finite.');
end

options = fill_default(options, 'flatlineToleranceUv', 0.02);
options = fill_default(options, 'maxFlatlineFraction', 0.20);
options = fill_default(options, 'clippingUv', 5000);
options = fill_default(options, 'maxClippingFraction', 0.001);
options = fill_default(options, 'maxLineNoiseRatio', 0.25);
options = fill_default(options, 'maxPeakToPeakUv', 2000);
options = fill_default(options, 'minStdUv', 0.05);

nChannels = size(x,2);
records = repmat(struct( ...
    'channel', 0, 'meanUv', NaN, 'stdUv', NaN, 'rmsUv', NaN, ...
    'peakToPeakUv', NaN, 'flatlineFraction', NaN, ...
    'nonfiniteFraction', NaN, 'clippingFraction', NaN, ...
    'lineNoiseRatio', NaN, 'bad', false, 'reasons', {{}}), 1, nChannels);

% Spectral diagnostic input: replace non-finite samples by channel medians.
spectralInput = x;
for ch = 1:nChannels
    finiteMask = isfinite(spectralInput(:,ch));
    if any(finiteMask)
        sorted = sort(spectralInput(finiteMask,ch));
        n = numel(sorted);
        if rem(n,2) == 1
            fillValue = sorted((n+1)/2);
        else
            fillValue = mean(sorted(n/2:n/2+1));
        end
        spectralInput(~finiteMask,ch) = fillValue;
    else
        spectralInput(:,ch) = 0;
    end
end
[f, p] = ads1299eeg.welch_psd(spectralInput, fs);
referenceHigh = min(100, fs/2 - eps);
refMask = f >= 1 & f <= referenceHigh;

for ch = 1:nChannels
    values = x(:,ch);
    finiteMask = isfinite(values);
    finiteValues = values(finiteMask);
    reasons = {};

    nonfiniteFraction = 1 - sum(finiteMask)/numel(values);
    if isempty(finiteValues)
        meanUv = NaN; stdUv = NaN; rmsUv = NaN; peakToPeakUv = NaN;
    else
        meanUv = mean(finiteValues);
        if numel(finiteValues) > 1
            stdUv = std(finiteValues, 0);
        else
            stdUv = 0;
        end
        rmsUv = sqrt(mean(finiteValues.^2));
        peakToPeakUv = max(finiteValues) - min(finiteValues);
    end

    if numel(values) > 1
        a = values(1:end-1);
        b = values(2:end);
        validDiff = isfinite(a) & isfinite(b);
        if any(validDiff)
            flatlineFraction = mean(abs(b(validDiff)-a(validDiff)) <= options.flatlineToleranceUv);
        else
            flatlineFraction = 1;
        end
    else
        flatlineFraction = 1;
    end

    if isempty(finiteValues)
        clippingFraction = 1;
    else
        clippingFraction = mean(abs(finiteValues) >= options.clippingUv);
    end

    if mainsHz < fs/2
        lineMask = f >= max(0,mainsHz-1) & f <= mainsHz+1;
        if nnz(lineMask) >= 2 && nnz(refMask) >= 2
            linePower = trapz(f(lineMask), p(lineMask,ch));
            referencePower = trapz(f(refMask), p(refMask,ch));
            if referencePower > 0
                lineNoiseRatio = linePower/referencePower;
            else
                lineNoiseRatio = NaN;
            end
        else
            lineNoiseRatio = NaN;
        end
    else
        lineNoiseRatio = NaN;
    end

    if nonfiniteFraction > 0, reasons{end+1} = 'nonfinite'; end %#ok<AGROW>
    if flatlineFraction > options.maxFlatlineFraction, reasons{end+1} = 'flatline'; end %#ok<AGROW>
    if clippingFraction > options.maxClippingFraction, reasons{end+1} = 'clipping'; end %#ok<AGROW>
    if isfinite(lineNoiseRatio) && lineNoiseRatio > options.maxLineNoiseRatio
        reasons{end+1} = 'line_noise'; %#ok<AGROW>
    end
    if isfinite(peakToPeakUv) && peakToPeakUv > options.maxPeakToPeakUv
        reasons{end+1} = 'large_peak_to_peak'; %#ok<AGROW>
    end
    if isfinite(stdUv) && stdUv < options.minStdUv
        reasons{end+1} = 'low_variance'; %#ok<AGROW>
    end
    if isempty(finiteValues), reasons{end+1} = 'no_finite_samples'; end %#ok<AGROW>

    records(ch).channel = ch;
    records(ch).meanUv = meanUv;
    records(ch).stdUv = stdUv;
    records(ch).rmsUv = rmsUv;
    records(ch).peakToPeakUv = peakToPeakUv;
    records(ch).flatlineFraction = flatlineFraction;
    records(ch).nonfiniteFraction = nonfiniteFraction;
    records(ch).clippingFraction = clippingFraction;
    records(ch).lineNoiseRatio = lineNoiseRatio;
    records(ch).bad = ~isempty(reasons);
    records(ch).reasons = reasons;
end

report.fs = fs;
report.nSamples = size(x,1);
report.nChannels = nChannels;
report.durationSeconds = size(x,1)/fs;
report.channels = records;
report.badChannelIndices = find([records.bad]);
end

function options = fill_default(options, field, value)
if ~isfield(options, field) || isempty(options.(field))
    options.(field) = value;
end
end
