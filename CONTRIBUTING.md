# Contributing

Thanks for your interest in Dreivy Engine.

This project is an experimental and learning-focused engine.
Contributions are welcome, including bug reports, suggestions,
and architectural discussions.

## What you can do

You are welcome to:
- report bugs
- suggest improvements
- propose architectural changes
- submit pull requests

## Please keep in mind

The project is under active development.
Internal APIs and implementation details may change frequently.

To keep development simple and fast, I may:
- directly integrate your code instead of merging the PR
- rewrite parts of the contribution to match the project style
- credit you in the commit message

This helps avoid unnecessary merge complexity and keeps the codebase clean.

## Code style and philosophy

When contributing, please follow these principles:

- Code should be easy to understand, even for beginners
- Avoid unnecessary abstractions or clever tricks
- No hidden logic or magic behavior
- Prefer explicit data flow
- Keep functions small and focused
- Comment *why*, not *what*

- If a piece of code is small, self-contained, and easy to read,
  it is perfectly fine to keep it header-only
  (for example: `Time.h`, `TransformUtils.h`).

- Header-only code is preferred when it improves readability,
  not when it hides complexity.

If a piece of code cannot be easily understood by someone new to the project,
it probably does not belong here.

## Discussion

If you are unsure about a change, feel free to open an issue first
to discuss the idea before implementing it.
