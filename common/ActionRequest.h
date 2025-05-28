
#ifndef ACTION_REQUEST_H
#define ACTION_REQUEST_H


//#include <string_view>

enum class ActionRequest {
    DoNothing,             // No action
    GetBattleInfo,
    MoveForward,      // 1 game step
    MoveBackward,     // Wait 2 steps, move on 3rd (unless canceled)
    RotateLeft90,   // 1 game step
    RotateRight90,  // 1 game step
    RotateLeft45,   // 1 game step
    RotateRight45,  // 1 game step
    Shoot             // 1 game step, 4-step cooldown after
};

// Optional: for debug/logging
inline const char* to_string(ActionRequest action) {
    switch (action) {
        case ActionRequest::DoNothing: return "None";
        case ActionRequest::GetBattleInfo: return "Get Battle Info";
        case ActionRequest::MoveForward: return "Move Forward";
        case ActionRequest::MoveBackward: return "Move Backward";
        case ActionRequest::RotateLeft90: return "Rotate Left 90 degrees";
        case ActionRequest::RotateRight90: return "Rotate Right 90 degrees";
        case ActionRequest::RotateLeft45: return "Rotate Left 45 degrees";
        case ActionRequest::RotateRight45: return "Rotate Right 45 degrees";
        case ActionRequest::Shoot: return "Shoot";
        default: return "Unknown";
    }
}

#endif // ACTION_REQUEST_H
