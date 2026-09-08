#!/usr/bin/env python3
"""Smoke test for feedback_controller.py."""

import math

from feedback_controller import FeedbackController


def main() -> None:
    controller = FeedbackController(smoothing_alpha=0.5, neutral_level=0.5)
    assert controller.output_level == 0.5

    update = controller.update(1.0)
    assert update.valid
    assert 0.5 < update.output_level <= 1.0

    update = controller.update(-1.0)
    assert update.valid
    assert 0.0 <= update.output_level <= 1.0
    assert update.input_score == 0.0

    update = controller.update(2.0)
    assert update.valid
    assert update.input_score == 1.0

    invalid = controller.update(None)
    assert not invalid.valid
    assert invalid.output_level == 0.5

    invalid = controller.update(float("nan"))
    assert not invalid.valid
    assert invalid.output_level == 0.5

    invalid = controller.update(0.9, signal_valid=False)
    assert not invalid.valid
    assert invalid.output_level == 0.5

    controller.reset()
    assert math.isclose(controller.output_level, 0.5)

    try:
        FeedbackController(smoothing_alpha=0.0)
        raise AssertionError("invalid alpha accepted")
    except ValueError:
        pass

    print("audio neurofeedback controller smoke test: PASS")


if __name__ == "__main__":
    main()
