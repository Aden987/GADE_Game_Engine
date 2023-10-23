#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class AnimationController {
public:
    //constructor - not really needed but c++ likes it so im leaving it here
    AnimationController() : time(0.0f) {}

    // Update the animation time (call this in your rendering loop after time variables are updated)
    // IF YOU DO NOT HAVE TIME VARIABLES....goodluck =) go look at the camera class tutorial they make use of it there
    void update(float deltaTime) {
        time += deltaTime;
    }
    
    // Rotation animation, this is a bit more generic, we can use speed and origin
    //PS you can change the origin of rotation, by passing in an extra vec3 variable when calling this function
    void applyRotationAnimation(glm::mat4& model, float speed, glm::vec3 origin = glm::vec3(0.0f, 1.0f, 0.0f)) {
        float rotationAngle = speed * time;
        model = glm::rotate(model, glm::radians(rotationAngle), origin);
    }

    // Apply Bouncing Animation, makes use of amplitude and frequency
    void applyBouncingAnimation(glm::mat4& model, float amplitude, float frequency) {
        float yOffset = amplitude * sin(time * frequency);
        model = glm::translate(model, glm::vec3(yOffset, 0.0f, 0.0f));
    }
    void applyYBouncingAnimation(glm::mat4& model, float amplitude, float frequency) {
        float yOffset = amplitude * sin(time * frequency);
        model = glm::translate(model, glm::vec3(0.0f,1.0f + yOffset, 0.0f));
    }
    //rotation animation but has a radius offset, NOTE: This can be done with translation + rotation, you do not need trig to do this
    //this is only if you have translation and DO NOT want to rotate based off of it
    void applyOffsetRotationAnimation(glm::mat4& model, float rotationSpeed, float offsetRadius) {
        //more mathy transformations
        float rotationAngle = time * rotationSpeed;
        //Trig for positioning
        float x = offsetRadius * cos(glm::radians(rotationAngle));
        float y = offsetRadius * sin(glm::radians(rotationAngle));

        model = glm::translate(model, glm::vec3(x, y, 0.0f));
        model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
    }

    void applyComplicatedAnimation(glm::mat4& model) {
        //lissajous figures, basically two sin waves mashed together to create a closed curve (repeatable, but complex)
        float A = 2.3f;    // Adjust as needed
        float B = 2.5f;    // Adjust as needed
        float omegaX = 2.0f;
        float omegaY = 3.0f;
        float phaseOffset = glm::radians(45.0f);  // Adjust as needed

        float x = A * sin(omegaX * time + phaseOffset);
        float y = B * sin(omegaY * time);
        model = glm::translate(model, glm::vec3(x, 1.0f + y, 0.0f));
    }

    void applyBreathingAnimation(glm::mat4& model, float speedMulti = 1, float amplitude = 1.0f) {
        float scale = 1.0f;
        scale = amplitude*sin(time * speedMulti);
        model = glm::scale(model, glm::vec3(scale, scale, scale));

    }

    void applyPawnAnimation(glm::mat4& model, float amplitude, float frequency, float yMovement) {
        float yOffset = amplitude * sin(time * frequency);
        model = glm::translate(model, glm::vec3(0.0f, yMovement +  yOffset, 0.0f));
        /*float xOffset = amplitude * sin(time * frequency);
        model = glm::translate(model, glm::vec3(xOffset, 0.0f, 0.0f));*/
        //model = glm::translate(model, glm::vec3(0.0f, 0.0f - yMovement, 0.0f) * time);
        //model = glm::translate(model, glm::vec3(0.0f, yMovement, 0.0f));
    }

    void applyKnightAnimation(glm::mat4& model, float amplitude, float frequency, float yMovement, float xMovement) {
        float yOffset = amplitude * sin(time * frequency);
        model = glm::translate(model, glm::vec3(0.0f,0.0f , yMovement + yOffset));
        float xOffset = amplitude * sin(time * frequency);
        model = glm::translate(model, glm::vec3(xMovement + xOffset, 0.0f, 0.0f));
        /*float xOffset = amplitude * sin(time * frequency);
        model = glm::translate(model, glm::vec3(xOffset, 0.0f, 0.0f));*/
        //model = glm::translate(model, glm::vec3(0.0f, 0.0f - yMovement, 0.0f) * time);
        //model = glm::translate(model, glm::vec3(0.0f, yMovement, 0.0f));
    }



private:
    float time;
};