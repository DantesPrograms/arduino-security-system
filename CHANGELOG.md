# Changelog

All notable changes to the Arduino R4 WiFi Security System will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Planned Features
- MQTT integration for IoT platforms
- Multiple user PIN support with admin/user roles
- Email/SMS notifications via external service
- Event logging to SD card
- Camera integration for visual verification
- Mobile application (iOS/Android)
- Zone-based multi-sensor support
- Alexa/Google Home integration
- Battery backup monitoring
- Tamper detection on enclosure

## [1.0.0] - 2026-01-30

### Added
- Initial release of Arduino R4 WiFi Security System
- PIR motion sensor integration for intrusion detection
- Three-state system architecture (DISARMED, ARMED, ALARM_TRIGGERED)
- Visual indicators using dual LED system (status + alarm)
- Audible alarm using active piezo buzzer with pulsing pattern
- Web-based control interface with responsive design
- PIN authentication system (4-digit PIN protection)
- Session management with 30-minute timeout
- RESTful API endpoints for programmatic control:
  - `GET /` - Main web interface
  - `POST /login` - Authentication
  - `GET /logout` - Session termination
  - `GET /status` - System state in JSON
  - `POST /arm` - Arm the system
  - `POST /disarm` - Disarm the system
- Auto-refreshing dashboard (3-second interval)
- Serial monitor debugging output at 115200 baud
- Real-time system state display with color coding
- Persistent alarm (continues until manually disarmed)
- WiFi connectivity using ESP32-S3 coprocessor
- Comprehensive documentation:
  - README.md with setup instructions
  - LICENSE.md (MIT License)
  - COPYRIGHT.md with attribution details
  - CONTRIBUTING.md for open source collaboration
  - pins.xml with complete circuit documentation

### Technical Details
- Platform: Arduino R4 WiFi (Renesas RA4M1 + ESP32-S3)
- WiFi Library: WiFiS3
- Pin Assignments:
  - D2: PIR sensor input
  - D8: Status LED output
  - D9: Alarm LED output
  - D10: Buzzer output
- Power Requirements: ~108mA maximum (all outputs active)
- Session Security: Token-based with timestamp validation
- HTTP Server: Built-in lightweight web server on port 80

### Known Limitations
- No HTTPS support (HTTP only)
- Single PIN authentication (no multi-user support)
- No persistent storage (settings reset on power cycle)
- No remote notifications
- No event logging
- 2.4GHz WiFi only (no 5GHz support)
- No over-the-air (OTA) update capability

### Hardware Requirements
- Arduino R4 WiFi board
- HC-SR501 PIR motion sensor (or compatible)
- 2x LEDs (3mm and 5mm recommended)
- 2x 220Ω resistors for LED current limiting
- 1x Active piezo buzzer (5V)
- Jumper wires and breadboard
- Optional: Enclosure for permanent installation

### Documentation
- Complete pin configuration and wiring diagram
- Bill of Materials with cost estimates
- Troubleshooting guide
- Safety considerations and warnings
- API documentation with curl examples
- Assembly and testing procedures

## Version History Summary

### Version Numbering Scheme
- **Major version** (X.0.0): Breaking changes or major feature additions
- **Minor version** (0.X.0): New features, backward compatible
- **Patch version** (0.0.X): Bug fixes and minor improvements

### Upcoming Versions (Planned)

#### [1.1.0] - Planned
- HTTPS support with SSL/TLS
- Persistent configuration storage using EEPROM
- Multiple alarm zones support
- Adjustable alarm patterns
- Enhanced logging to Serial output

#### [1.2.0] - Planned
- MQTT integration for home automation
- Time-based arm/disarm scheduling
- Low power mode for battery operation
- Over-the-air (OTA) firmware updates

#### [2.0.0] - Planned (Breaking Changes)
- Complete REST API redesign
- Multiple user accounts with roles
- Database integration for event logging
- Cloud connectivity option
- Breaking: New authentication mechanism
- Breaking: Modified pin assignments for expanded I/O

## Migration Guides

### Future: Migrating from 1.x to 2.0
*(Will be provided when 2.0 is released)*

Expected breaking changes:
- Authentication system redesign
- API endpoint restructuring
- Configuration storage format change
- Pin assignment modifications

## Release Notes Format

Each release includes:
- **Date**: Release date in YYYY-MM-DD format
- **Added**: New features and capabilities
- **Changed**: Changes to existing functionality
- **Deprecated**: Features marked for removal
- **Removed**: Features removed from this release
- **Fixed**: Bug fixes and corrections
- **Security**: Security vulnerability fixes

## Bug Tracking

Report bugs via GitHub Issues:
- Use issue templates provided
- Include Arduino IDE version
- Provide Serial Monitor output
- Describe steps to reproduce
- Include hardware setup details

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md) for:
- Development workflow
- Coding standards
- Testing requirements
- Pull request process

## Support

For questions and support:
- GitHub Issues: Bug reports and feature requests
- GitHub Discussions: Questions and community support
- Documentation: Check README.md and other docs

## Links

- **Repository**: https://github.com/yourusername/arduino-security-system
- **Documentation**: See README.md
- **License**: MIT License (see LICENSE.md)
- **Author**: Dante D'Abramo

---

*This changelog follows the [Keep a Changelog](https://keepachangelog.com/) format.*  
*Last Updated: January 30, 2026*
