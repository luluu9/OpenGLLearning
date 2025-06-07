#version 330 core
// Description: Fragment shader for highlighting selected objects

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

// Highlight color and parameters
uniform vec4 highlightColor;
uniform float time; // Global time for animations

void main()
{
    // Edges glow more than center
    vec3 viewDir = normalize(-FragPos);
    float rim = 1.0 - max(dot(viewDir, normalize(Normal)), 0.0);
    rim = pow(rim, 3.0);
    
    // Add pulsing effect
    float pulse = 0.5 + 0.5 * sin(time * 3.0); // Adjust speed with multiplier
    
    // Combine rim and pulse for final highlight effect
    vec4 finalColor = highlightColor;
    finalColor.a *= rim * pulse;
    
    FragColor = finalColor;
}
