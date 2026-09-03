# Brodmann Areas (BA1–BA52)

This subdirectory is the classical cortical cytoarchitecture learning track of ADS1299-Core-SDK.

## Important scientific caution

“Brodmann areas” are **cytoarchitectonic cortical areas**, not a perfect modern functional atlas and not a one-electrode-to-one-function lookup table. The familiar BA numbers are useful historical and educational coordinates, but functional boundaries vary across individuals and modern neuroimaging often uses newer anatomical, probabilistic and network-based atlases.

The project should therefore use Brodmann areas as one layer of interpretation, not as the only description of brain organization.

## Coverage plan

The long-term index is BA1 through BA52. Each area can eventually receive a dedicated page when the evidence base is checked carefully.

```text
BA01 BA02 BA03 BA04 BA05 BA06 BA07 BA08 BA09 BA10
BA11 BA12 BA13 BA14 BA15 BA16 BA17 BA18 BA19 BA20
BA21 BA22 BA23 BA24 BA25 BA26 BA27 BA28 BA29 BA30
BA31 BA32 BA33 BA34 BA35 BA36 BA37 BA38 BA39 BA40
BA41 BA42 BA43 BA44 BA45 BA46 BA47 BA48 BA49 BA50
BA51 BA52
```

Not every number should be treated as an equally well-defined or equally important human EEG target. Future pages must explain historical/anatomical caveats rather than forcing every BA into a simplistic functional label.

## Template for each area

A mature BA page should answer:

1. **Where is it?** Lobe, gyrus/sulcus, medial/lateral surface, nearby areas.
2. **What is the classical cytoarchitectonic definition?**
3. **What are its strongest modern functional associations?**
4. **Which major inputs/outputs or networks are relevant?**
5. **Which EEG/ERP/BCI paradigms are commonly associated with this region?**
6. **Can scalp EEG observe it directly?** Usually only indirectly; explain depth/orientation/volume-conduction limits.
7. **Which 10–20 / 10–10 positions are often near the scalp projection?** Mark these as approximate, not exact.
8. **Which experimental tasks or clinical questions commonly study it?**
9. **What are the common interpretation mistakes?**
10. **What evidence level supports each claim?** Classical anatomy, imaging association, lesion evidence, stimulation, intracranial evidence, etc.

## High-value EEG/BCI groups to prioritize

Future maintenance will likely prioritize groups that commonly appear in EEG/BCI research, for example:

- primary sensorimotor cortex and premotor/SMA-related areas;
- visual cortex and visual association cortex;
- auditory cortex;
- prefrontal and frontal control areas;
- language-related frontal/temporal regions;
- parietal attention and visuospatial regions;
- cingulate/medial frontal areas relevant to error monitoring, attention and affect.

This is a prioritization strategy, not a statement that other Brodmann areas are unimportant.

## Relationship to EEG electrodes

Do not write rules such as “F3 = BA9” as if they are anatomically exact. A scalp location can overlap different gyri/BAs across people, and each EEG channel reflects a spatially mixed electric field.

Use [`../electrode_cortex_mapping/`](../electrode_cortex_mapping/) for evidence-backed approximate mappings and localization caveats.

## Source direction

Future BA-specific pages should use the original Brodmann cytoarchitectonic work as historical grounding plus modern peer-reviewed atlases, neuroimaging, stimulation, lesion and electrophysiology evidence where appropriate. Functional claims should not be copied from unsourced popular brain maps.