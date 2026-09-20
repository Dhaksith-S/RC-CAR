#pragma once

// Registers routes and starts the HTTP server. Call once from setup(),
// after motorControlInit() and wifiApInit().
void httpServerInit();

// Call every loop() iteration to process incoming HTTP requests.
void httpServerHandle();

// Call every loop() iteration. Stops the motors if no valid command has
// arrived within COMMAND_TIMEOUT_MS while the car is moving.
void failsafeCheck();
