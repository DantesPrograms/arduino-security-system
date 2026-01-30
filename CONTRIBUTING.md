# Contributing to Arduino R4 WiFi Security System

Thank you for your interest in contributing to this project! This document provides guidelines and information for contributors.

## 📋 Table of Contents

- [Code of Conduct](#code-of-conduct)
- [How Can I Contribute?](#how-can-i-contribute)
- [Development Setup](#development-setup)
- [Coding Standards](#coding-standards)
- [Submission Guidelines](#submission-guidelines)
- [Testing Requirements](#testing-requirements)
- [Documentation Standards](#documentation-standards)

## 🤝 Code of Conduct

### Our Pledge

We are committed to providing a welcoming and inclusive experience for everyone, regardless of:
- Level of experience
- Gender identity and expression
- Sexual orientation
- Disability
- Personal appearance
- Body size
- Race
- Ethnicity
- Age
- Religion
- Nationality

### Our Standards

**Positive behaviors include:**
- Using welcoming and inclusive language
- Being respectful of differing viewpoints
- Gracefully accepting constructive criticism
- Focusing on what is best for the community
- Showing empathy towards others

**Unacceptable behaviors include:**
- Harassment or discriminatory comments
- Personal or political attacks
- Public or private harassment
- Publishing others' private information without permission
- Other conduct inappropriate in a professional setting

## 💡 How Can I Contribute?

### Reporting Bugs

Before submitting a bug report:
1. Check existing issues to avoid duplicates
2. Test with the latest version of the code
3. Verify it's not a hardware connection issue

**Bug Report Template:**
```markdown
**Description:** Clear description of the bug

**Steps to Reproduce:**
1. Step one
2. Step two
3. ...

**Expected Behavior:** What should happen

**Actual Behavior:** What actually happens

**Environment:**
- Arduino IDE version:
- Board: Arduino R4 WiFi
- OS: Windows/Mac/Linux
- WiFi router model (if relevant):

**Serial Monitor Output:** 
```
[Paste relevant output]
```

**Additional Context:** Screenshots, videos, or other info
```

### Suggesting Enhancements

Enhancement suggestions are welcome! Please provide:
- Clear use case for the feature
- Expected behavior
- Why this would benefit users
- Potential implementation approach (if known)

**Popular Enhancement Ideas:**
- Multiple user PIN support
- Email/SMS notifications
- MQTT integration for IoT platforms
- SD card event logging
- Camera integration
- Mobile app development
- Zone-based detection
- Advanced alarm patterns

### Pull Requests

We actively welcome pull requests for:
- Bug fixes
- New features
- Documentation improvements
- Code optimization
- Hardware compatibility additions
- Test coverage improvements

## 🛠️ Development Setup

### Prerequisites

1. **Arduino IDE** (2.0 or higher)
2. **Arduino R4 WiFi Board Support**
3. **Git** for version control
4. **Text editor** (VS Code recommended)

### Initial Setup

```bash
# 1. Fork the repository on GitHub

# 2. Clone your fork
git clone https://github.com/YOUR-USERNAME/arduino-security-system.git
cd arduino-security-system

# 3. Add upstream remote
git remote add upstream https://github.com/ORIGINAL-OWNER/arduino-security-system.git

# 4. Create a development branch
git checkout -b feature/my-new-feature
```

### Hardware Setup

You'll need the complete hardware setup as described in README.md:
- Arduino R4 WiFi
- PIR sensor
- LEDs and resistors
- Buzzer
- Connecting wires

## 📝 Coding Standards

### General Guidelines

- **Readability:** Code should be self-documenting with clear variable names
- **Comments:** Explain *why*, not *what* (code shows what)
- **Functions:** Keep functions focused and under 50 lines when possible
- **Constants:** Use `const` for values that don't change
- **Magic Numbers:** Avoid hard-coded numbers; use named constants

### Naming Conventions

```cpp
// Constants: UPPER_CASE with underscores
const int PIR_PIN = 2;
const unsigned long SESSION_TIMEOUT = 1800000;

// Variables: camelCase
int currentState = DISARMED;
bool motionDetected = false;

// Functions: camelCase
void handleWebClient() { }
void armSystem(WiFiClient& client) { }

// Classes/Structs: PascalCase
struct SystemConfig { };
enum SystemState { };
```

### Code Structure

```cpp
// 1. Header comments
/*
 * File: feature_name.ino
 * Description: Brief description
 * Author: Your Name
 * Date: YYYY-MM-DD
 */

// 2. Includes
#include <WiFiS3.h>

// 3. Constants
const int PIN_NUMBER = 10;

// 4. Global variables
int globalVar = 0;

// 5. Setup function
void setup() {
  // Initialization
}

// 6. Main loop
void loop() {
  // Main logic
}

// 7. Helper functions
void helperFunction() {
  // Implementation
}
```

### Documentation Comments

```cpp
/**
 * Handles incoming web client requests
 * 
 * Parses HTTP headers, manages authentication,
 * and routes requests to appropriate handlers
 * 
 * @note This function blocks until client disconnects
 */
void handleWebClient() {
  // Implementation
}
```

## 📤 Submission Guidelines

### Before Submitting

- [ ] Code follows project coding standards
- [ ] All tests pass (if applicable)
- [ ] Documentation updated
- [ ] Commit messages are clear
- [ ] No merge conflicts with main branch

### Commit Messages

Follow conventional commit format:

```
type(scope): brief description

Longer explanation if needed

Fixes #123
```

**Types:**
- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation changes
- `style`: Code formatting (no logic change)
- `refactor`: Code restructuring
- `test`: Adding tests
- `chore`: Maintenance tasks

**Examples:**
```
feat(web): add HTTPS support for web interface

Implements SSL/TLS encryption for secure communication.
Uses self-signed certificates for development.

Fixes #45
```

```
fix(pir): resolve false triggers during startup

Added 60-second stabilization delay after power-on
to prevent PIR sensor false positives.

Closes #23
```

### Pull Request Process

1. **Update documentation** if adding features
2. **Add tests** if applicable
3. **Update README.md** with new capabilities
4. **Update CHANGELOG.md** with changes
5. **Ensure Arduino IDE compilation** succeeds
6. **Test on actual hardware** if possible

**Pull Request Template:**
```markdown
## Description
Brief description of changes

## Type of Change
- [ ] Bug fix
- [ ] New feature
- [ ] Breaking change
- [ ] Documentation update

## Testing
Describe testing performed:
- [ ] Compiled successfully
- [ ] Tested on hardware
- [ ] All features work as expected

## Checklist
- [ ] Code follows style guidelines
- [ ] Self-review completed
- [ ] Comments added for complex code
- [ ] Documentation updated
- [ ] No new warnings generated

## Screenshots/Videos
[If applicable]

## Related Issues
Fixes #[issue number]
```

## 🧪 Testing Requirements

### Hardware Testing

Before submitting hardware-related changes:
1. Test on actual Arduino R4 WiFi board
2. Verify all connections work correctly
3. Test edge cases (weak WiFi, sensor obstruction, etc.)
4. Confirm power consumption is acceptable
5. Check long-term stability (run for several hours)

### Software Testing

Test these scenarios:
- [ ] System boots correctly
- [ ] WiFi connection succeeds
- [ ] Web interface loads properly
- [ ] PIN authentication works
- [ ] ARM/DISARM functions work
- [ ] PIR sensor detects motion
- [ ] Alarm activates correctly
- [ ] Alarm only stops when disarmed
- [ ] Session timeout works
- [ ] Invalid PIN is rejected
- [ ] System handles WiFi disconnection

### Test Documentation

Document your testing:
```markdown
## Test Results

**Environment:**
- Arduino IDE: 2.3.2
- Board: Arduino R4 WiFi
- WiFi: 2.4GHz WPA2
- Browser: Chrome 120

**Test Cases:**
| Test Case | Result | Notes |
|-----------|--------|-------|
| Boot sequence | ✅ Pass | Serial output correct |
| WiFi connection | ✅ Pass | Connected in 5s |
| PIN auth | ✅ Pass | Accepted valid PIN |
| Motion detection | ✅ Pass | Triggered within 2s |
| Alarm persistence | ✅ Pass | Continued until disarmed |

**Issues Found:** None
```

## 📚 Documentation Standards

### README Updates

When adding features:
- Update feature list
- Add configuration instructions
- Update API documentation if applicable
- Add troubleshooting tips
- Include usage examples

### Code Comments

```cpp
// Good: Explains why
// Using 500ms pulse to create audible alarm pattern
// that's noticeable but not overwhelming

// Bad: Explains what (obvious from code)
// Turn buzzer on
digitalWrite(BUZZER_PIN, HIGH);
```

### Inline Documentation

For complex algorithms:
```cpp
/**
 * State Machine: ALARM_TRIGGERED
 * 
 * When motion is detected in ARMED state:
 * 1. Activate visual indicators (LEDs)
 * 2. Start audible alarm (buzzer)
 * 3. Continue until explicitly disarmed
 * 
 * Design Decision: No automatic timeout ensures
 * alarm persists until user acknowledgment via
 * authenticated web interface.
 */
void handleAlarmState() {
  // Implementation
}
```

## 🎨 Hardware Contributions

### New Sensor Support

When adding new sensors:
1. Document pin connections in `pins.xml`
2. Update wiring diagram
3. Add to BOM (Bill of Materials)
4. Document calibration procedure
5. Add troubleshooting section

### Circuit Modifications

For circuit changes:
1. Test thoroughly on breadboard
2. Calculate power requirements
3. Verify GPIO current limits
4. Update schematic documentation
5. Provide before/after comparison

## 📞 Getting Help

- **Questions:** Open a GitHub discussion
- **Bugs:** Create an issue with details
- **Ideas:** Start a discussion or create feature request
- **Urgent:** Tag maintainers in issue/PR

## 🏆 Recognition

Contributors will be:
- Listed in project README
- Credited in release notes
- Thanked in documentation

### Hall of Fame

Outstanding contributions may receive:
- Spotlight in README
- Special mention in blog posts
- Invitation to maintainer team

## 📄 License

By contributing, you agree that your contributions will be licensed under the MIT License.

---

**Thank you for contributing to making this project better!** 🙏

*Last Updated: January 30, 2026*
