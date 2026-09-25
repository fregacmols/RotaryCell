# RotaryCell illustrated build guide

This directory contains the standalone, photographed construction guide for
the current RotaryCell PCB build. The repository's engineering records remain
in the top-level `docs/`, `hardware/`, `firmware/`, and `development/`
directories and are not mixed into the guide's navigation.

The published guide is available at <https://evilroot.net/RotaryCell/>. GitHub
Pages is generated from this directory by `.github/workflows/guide-pages.yml`
whenever guide source changes on `main`; the repository's custom domain serves
that build at the address above.

## Contents

- `docs/` contains the guide pages, photographs, diagrams, downloads, and CSS.
- `overrides/` contains the small Material theme override used for the guide banner.
- `mkdocs.yml` defines navigation and site settings.
- `requirements.txt` pins the MkDocs versions used by the publishing workflow.

## Build locally

From the repository root:

```text
python -m pip install -r guide/requirements.txt
python -m mkdocs build --strict -f guide/mkdocs.yml
```

The generated site is written to `guide/site-build/` and is intentionally not
committed. The illustrated guide is the recommended construction path; the
older top-level engineering documents remain available as historical and
technical reference material.

Corrections and improvements can be submitted through the repository.
