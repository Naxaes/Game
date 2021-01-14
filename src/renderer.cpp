
class Renderer
{
    void begin_scene(const Camera& camera);
    void end_scene();
    void flush();
};



void Renderer:::begin_scene(const Camera& camera)
{
    auto view       = ViewMatrix(camera.position, camera.forward);
    auto projection = camera.perspective_projection();

    SetUniform(shader, "view",       view);
    SetUniform(shader, "projection", projection);
};
void Renderer:::render_mesh(const Software& mesh)
{
    this->vertices
};
void Renderer:::end_scene()
{

};
void Renderer:::flush()
{
    glBindVertexArray(renderable.mesh->id);
    SetTexture2D(shader, "diffuse", 0, renderable.mesh->texture);
    glDrawArrays(GL_TRIANGLES, 0, renderable.mesh->count);
};