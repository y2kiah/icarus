void PSMain(out float4 colorOut : SV_Target)
{
    // Make each pixel yellow, with alpha = 1
    colorOut = float4(1.0f, 1.0f, 0.0f, 1.0f);
}