#ifndef RENDERINGENGINE_H_
#define RENDERINGENGINE_H_

/* This class will be a singleton responsible for all rendering related
   stuff */

class RenderingEngine {
    public:
        /* Singleton instancing of this object 
        --> avoids making everything static */
        static RenderingEngine& get_instance() {
            static RenderingEngine instance;
            return instance;
        }

    private:
        /* Private constructor blocks any object from being made */
        RenderingEngine() {}

    public:
        /* Provides dynamic deletion in case of illegal reference */
        RenderingEngine(RenderingEngine const&) = delete;
        void operator=(RenderingEngine const&) = delete;

};

#endif