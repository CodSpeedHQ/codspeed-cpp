.PHONY: format
format:
	@echo "Formatting C++ files..."
	@git ls-files '*.cpp' '*.h' | grep -v '^google_benchmark/' | xargs -r clang-format -i
	@echo "Formatting complete!"
