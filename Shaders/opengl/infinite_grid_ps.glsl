#version 450

layout(location = 0) in vec2 in_var_TEXCOORD0;
layout(location = 1) in vec2 in_var_TEXCOORD1;
layout(location = 0) out vec4 out_var_SV_Target;

void main()
{
    float _35 = dFdx(in_var_TEXCOORD0.x);
    float _36 = dFdy(in_var_TEXCOORD0.x);
    float _40 = dFdx(in_var_TEXCOORD0.y);
    float _41 = dFdy(in_var_TEXCOORD0.y);
    vec2 _44 = vec2(length(vec2(_35, _36)), length(vec2(_40, _41)));
    float _49 = (log2(length(_44) * 160.0) * 0.3010300099849700927734375) + 1.0;
    float _50 = isnan(_49) ? 0.0 : (isnan(0.0) ? _49 : max(0.0, _49));
    float _51 = fract(_50);
    float _53 = pow(10.0, floor(_50));
    vec2 _57 = _44 * 4.0;
    vec2 _59 = in_var_TEXCOORD0 + (_57 * vec2(0.5));
    vec2 _60 = vec2(0.02500000037252902984619140625 * _53);
    vec2 _67 = vec2(1.0) - abs((clamp((_59 - _60 * trunc(_59 / _60)) / _57, vec2(0.0), vec2(1.0)) * 2.0) - vec2(1.0));
    float _68 = _67.x;
    float _69 = _67.y;
    vec2 _71 = vec2(_53 * 0.25);
    vec2 _78 = vec2(1.0) - abs((clamp((_59 - _71 * trunc(_59 / _71)) / _57, vec2(0.0), vec2(1.0)) * 2.0) - vec2(1.0));
    float _79 = _78.x;
    float _80 = _78.y;
    float _81 = isnan(_80) ? _79 : (isnan(_79) ? _80 : max(_79, _80));
    vec2 _82 = vec2(_53 * 2.5);
    vec2 _89 = vec2(1.0) - abs((clamp((_59 - _82 * trunc(_59 / _82)) / _57, vec2(0.0), vec2(1.0)) * 2.0) - vec2(1.0));
    float _90 = _89.x;
    float _91 = _89.y;
    float _92 = isnan(_91) ? _90 : (isnan(_90) ? _91 : max(_90, _91));
    bool _94 = _92 > 0.0;
    vec4 _105;
    if (_94)
    {
        _105 = vec4(0.0, 0.0, 0.0, 1.0);
    }
    else
    {
        vec4 _104;
        if (_81 > 0.0)
        {
            _104 = mix(vec4(0.0, 0.0, 0.0, 1.0), vec4(0.5, 0.5, 1.0, 1.0), vec4(_51));
        }
        else
        {
            _104 = vec4(0.5, 0.5, 1.0, 1.0);
        }
        _105 = _104;
    }
    float _120;
    if (_94)
    {
        _120 = _92;
    }
    else
    {
        float _119;
        if (_81 > 0.0)
        {
            _119 = _81;
        }
        else
        {
            _119 = (isnan(_69) ? _68 : (isnan(_68) ? _69 : max(_68, _69))) * (1.0 - _51);
        }
        _120 = _119;
    }
    vec4 _124 = _105;
    _124.w = _105.w * (_120 * (1.0 - clamp(length(_59 - in_var_TEXCOORD1) * 0.00999999977648258209228515625, 0.0, 1.0)));
    out_var_SV_Target = _124;
}

