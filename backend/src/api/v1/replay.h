#pragma once

#include <crow.h>

crow::response api_replay_get(const crow::request& request);
crow::response api_replay_post(const crow::request& request);
crow::response api_replay_data_get(const crow::request& request);
