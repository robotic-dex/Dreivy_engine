#pragma once
#include <vector>

// forward declaration
struct Transform;

struct RenderItem {
    const Transform* transform;
    int meshId;
};

class RenderQueue {
public:
    void Submit(const Transform& t, int meshId) {
        items.push_back({ &t, meshId });
    }

    const std::vector<RenderItem>& GetItems() const {
        return items;
    }

    void Clear() {
        items.clear();
    }

private:
    std::vector<RenderItem> items;
};
