#pragma once

#include <glm.hpp>

# define PI           3.1415926535f

class Material
{
public:
    // Disney brdf params
    glm::vec3 baseColor; // .82 .67 .16
    float metallic; // 0 1 0
    float subsurface; // 0 1 0
    float specular; // 0 1 .5
    float roughness; // 0 1 .5
    float specularTint; // 0 1 0
    float anisotropic; // 0 1 0
    float sheen; // 0 1 0
    float sheenTint; // 0 1 .5
    float clearcoat; // 0 1 0
    float clearcoatGloss; // 0 1 1

    // Oren-nayar param
    float sigma;

    // Constructor
	Material()
	{
        // For Disney brdf
        baseColor = glm::vec3(0.8f, 0.8f, 0.8f); // (0.82f, 0.67f, 0.16f)
        metallic = 0.f; // 0 1 0
        subsurface = 0.5f; // 0 1 0
        specular = 0.5f; // 0 1 .5
        roughness = .5f; // 0 1 .5
        specularTint = 0.f; // 0 1 0
        anisotropic = 0.f; // 0 1 0
        sheen = 0.f; // 0 1 0
        sheenTint = .5f; // 0 1 .5
        clearcoat = 0.65f; // 0 1 0
        clearcoatGloss = 1.f; // 0 1 1

        // For Oren-nayar
        sigma = 30; //0 90 30
	}

    //// Lambert BRDF
    //glm::vec3 BRDF(glm::vec3 L, glm::vec3 V, glm::vec3 N, glm::vec3 X, glm::vec3 Y)
    //{
    //    return this->baseColor/PI;
    //}

    //// Oren-nayar BRDF
    //glm::vec3 BRDF(glm::vec3 L, glm::vec3 V, glm::vec3 N, glm::vec3 X, glm::vec3 Y)
    //{
    //    float VdotN = dot(V, N);
    //    float LdotN = dot(L, N);
    //    float theta_r = acos(VdotN);
    //    float sigma2 = pow(sigma * PI / 180, 2);

    //    float cos_phi_diff = dot(normalize(V - N * (VdotN)), normalize(L - N * (LdotN)));
    //    float theta_i = acos(LdotN);
    //    float alpha = std::max(theta_i, theta_r);
    //    float beta = std::min(theta_i, theta_r);
    //    if (alpha > PI / 2) return glm::vec3(0);

    //    float C1 = 1 - 0.5 * sigma2 / (sigma2 + 0.33);
    //    float C2 = 0.45 * sigma2 / (sigma2 + 0.09);
    //    if (cos_phi_diff >= 0) C2 *= sin(alpha);
    //    else C2 *= (sin(alpha) - pow(2 * beta / PI, 3));
    //    float C3 = 0.125 * sigma2 / (sigma2 + 0.09) * pow((4 * alpha * beta) / (PI * PI), 2);
    //    glm::vec3 L1 = this->baseColor / float(PI) * (C1 + cos_phi_diff * C2 * tan(beta) + (1 - abs(cos_phi_diff)) * C3 * tan((alpha + beta) / 2));
    //    glm::vec3 L2 = 0.17f * this->baseColor * this->baseColor / float(PI) * sigma2 / (sigma2 + 0.13f) * (1.f - cos_phi_diff * (4.f * beta * beta) / float(PI * PI));
    //    return glm::vec3(L1 + L2);
    //}

	//// Disney brdf code
    float sqr(float x) { return x * x; }

    float clamp(float d, float min, float max) {
        const float t = d < min ? min : d;
        return t > max ? max : t;
    }

    float mix(float v1, float v2, float a)
    {
        return v1 * (1 - a) + v2 * a;
    }

    glm::vec3 mix(glm::vec3 v1, glm::vec3 v2, float a)
    {
        return v1 * (1 - a) + v2 * a;
    }

    float SchlickFresnel(float u)
    {
        float m = clamp(1 - u, 0.f, 1.f);
        float m2 = m * m;
        return m2 * m2 * m; // pow(m,5)
    }

    float GTR1(float NdotH, float a)
    {
        if (a >= 1) return 1 / PI;
        float a2 = a * a;
        float t = 1 + (a2 - 1) * NdotH * NdotH;
        return (a2 - 1) / (PI * log(a2) * t);
    }

    float GTR2(float NdotH, float a)
    {
        float a2 = a * a;
        float t = 1 + (a2 - 1) * NdotH * NdotH;
        return a2 / (PI * t * t);
    }

    float GTR2_aniso(float NdotH, float HdotX, float HdotY, float ax, float ay)
    {
        return 1 / (PI * ax * ay * sqr(sqr(HdotX / ax) + sqr(HdotY / ay) + NdotH * NdotH));
    }

    float smithG_GGX(float NdotV, float alphaG)
    {
        float a = alphaG * alphaG;
        float b = NdotV * NdotV;
        return 1 / (NdotV + sqrt(a + b - a * b));
    }

    float smithG_GGX_aniso(float NdotV, float VdotX, float VdotY, float ax, float ay)
    {
        return 1 / (NdotV + sqrt(sqr(VdotX * ax) + sqr(VdotY * ay) + sqr(NdotV)));
    }

    glm::vec3 mon2lin(glm::vec3 x)
    {
        return glm::vec3(pow(x[0], 2.2), pow(x[1], 2.2), pow(x[2], 2.2));
    }


    glm::vec3 BRDF(glm::vec3 L, glm::vec3 V, glm::vec3 N, glm::vec3 X, glm::vec3 Y)
    {
        float NdotL = glm::dot(N, L);
        float NdotV = glm::dot(N, V);
        if (NdotL < 0 || NdotV < 0) return glm::vec3(0);

        glm::vec3 H = glm::normalize(L + V);
        float NdotH = glm::dot(N, H);
        float LdotH = glm::dot(L, H);

        glm::vec3 Cdlin = mon2lin(baseColor);
        float Cdlum = .3f * Cdlin[0] + .6f * Cdlin[1] + .1f * Cdlin[2]; // luminance approx.

        glm::vec3 Ctint = Cdlum > 0 ? Cdlin / Cdlum : glm::vec3(1.f); // normalize lum. to isolate hue+sat
        glm::vec3 Cspec0 = mix(specular * .08f * mix(glm::vec3(1.f), Ctint, specularTint), Cdlin, metallic);
        glm::vec3 Csheen = mix(glm::vec3(1.f), Ctint, sheenTint);

        // Diffuse fresnel - go from 1 at normal incidence to .5 at grazing
        // and mix in diffuse retro-reflection based on roughness
        float FL = SchlickFresnel(NdotL), FV = SchlickFresnel(NdotV);
        float Fd90 = 0.5f + 2.f * LdotH * LdotH * roughness;
        float Fd = mix(1.f, Fd90, FL) * mix(1.f, Fd90, FV);

        // Based on Hanrahan-Krueger brdf approximation of isotropic bssrdf
        // 1.25 scale is used to (roughly) preserve albedo
        // Fss90 used to "flatten" retroreflection based on roughness
        float Fss90 = LdotH * LdotH * roughness;
        float Fss = mix(1.0f, Fss90, FL) * mix(1.0f, Fss90, FV);
        float ss = 1.25f * (Fss * (1.f / (NdotL + NdotV) - .5f) + .5f);

        // specular
        float aspect = sqrt(1.f - anisotropic * .9f);
        float ax = std::max(.001f, sqr(roughness) / aspect);
        float ay = std::max(.001f, sqr(roughness) * aspect);
        float Ds = GTR2_aniso(NdotH, dot(H, X), dot(H, Y), ax, ay);
        float FH = SchlickFresnel(LdotH);
        glm::vec3 Fs = mix(Cspec0, glm::vec3(1.f), FH);
        float Gs;
        Gs = smithG_GGX_aniso(NdotL, dot(L, X), dot(L, Y), ax, ay);
        Gs *= smithG_GGX_aniso(NdotV, dot(V, X), dot(V, Y), ax, ay);

        // sheen
        glm::vec3 Fsheen = FH * sheen * Csheen;

        // clearcoat (ior = 1.5 -> F0 = 0.04)
        float Dr = GTR1(NdotH, mix(.1f, .001f, clearcoatGloss));
        float Fr = mix(.04f, 1.0f, FH);
        float Gr = smithG_GGX(NdotL, .25f) * smithG_GGX(NdotV, .25f);

        return ((1 / PI) * mix(Fd, ss, subsurface) * Cdlin + Fsheen)
            * (1 - metallic)
            + Gs * Fs * Ds + .25f * clearcoat * Gr * Fr * Dr;
    }
};