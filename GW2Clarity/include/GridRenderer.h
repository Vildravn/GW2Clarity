#pragma once

#include "Buffs.h"
#include "Graphics.h"
#include "ShaderManager.h"

namespace GW2Clarity
{
struct GridInstanceData
{
    vec4 posDims;
    vec2 uv;
    vec2 numberUV;
    vec4 tint;
    vec4 borderColor;
    vec4 glowColor;
    vec2 glowSize;
    f32 borderThickness;
    i32 showNumber;
};

class BaseGridRenderer
{
    using InstanceData = GridInstanceData;

public:
    BaseGridRenderer(ComPtr<ID3D11Device>& dev, const Buffs* buffs, size_t sz);

protected:
    void Draw(ComPtr<ID3D11DeviceContext>& ctx, std::span<InstanceData> data, bool betterFiltering, RenderTarget* rt, bool expandVS);

    const Buffs* buffs_;

    ShaderId screenSpaceVS_;
    ShaderId screenSpaceNoExpandVS_;
    ShaderId gridsPS_;
    ShaderId gridsFilteredPS_;

    struct GridConstants
    {
        vec4 screenSize;
        vec2 atlasUVSize;
        vec2 numbersUVSize;
        f32 time;
    };
    ConstantBufferSPtr<GridConstants> gridCB_;

    ComPtr<ID3D11Buffer> instanceBuffer_;
    ComPtr<ID3D11ShaderResourceView> instanceBufferView_;
    ComPtr<ID3D11BlendState> defaultBlend_;
    ComPtr<ID3D11SamplerState> defaultSampler_;

    size_t bufferSize_;
};

template<size_t N>
class GridRenderer : public BaseGridRenderer
{
    using InstanceData = GridInstanceData;

public:
    GridRenderer(ComPtr<ID3D11Device>& dev, const Buffs* buffs) : BaseGridRenderer(dev, buffs, N) { }
    GridRenderer(const GridRenderer&) = delete;
    GridRenderer(GridRenderer&&) = delete;
    GridRenderer& operator=(const GridRenderer&) = delete;
    GridRenderer& operator=(GridRenderer&&) = delete;
    ~GridRenderer() = default;

    void Add(InstanceData&& data) {
        if(instanceBufferCount_ >= instanceBufferSize_s)
            return;

        instanceBufferSource_[instanceBufferCount_++] = std::move(data);
    }

    void Draw(ComPtr<ID3D11DeviceContext>& ctx, bool betterFiltering, RenderTarget* rt = nullptr, bool expandVS = true) {
        BaseGridRenderer::Draw(ctx, std::span { instanceBufferSource_.begin(), instanceBufferCount_ }, betterFiltering, rt, expandVS);
        instanceBufferCount_ = 0;
    }

protected:
    static constexpr size_t instanceBufferSize_s = N;
    std::array<InstanceData, instanceBufferSize_s> instanceBufferSource_ {};
    u32 instanceBufferCount_ = 0;
};
} // namespace GW2Clarity
