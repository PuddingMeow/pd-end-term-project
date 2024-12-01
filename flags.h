#include <string>
#include <stdexcept>

class file_error : public std::runtime_error {
    private:
    public:
        file_error(std::string msg) : runtime_error(msg) {};
};

struct Flag {
    std::string name;
    bool status;
};

class FlagArray{
    private:
        Flag* flag;
        int flagCnt;

        Flag* linearSearch(std::string flagName) const;
    public:
        FlagArray();
        FlagArray(const FlagArray&);
        ~FlagArray();

        bool checkFlagStatus(std::string flagName) const;
        void updateFlagStatus(std::string flagName, bool status);
        void print() const; // debug.
};