// yafth
#include "yafth/random.h"
#include "yafth/engine.h"
// stl
#include <algorithm>
#include <numeric>
#include <utility>

#include <sstream>

namespace
{
    // constexpr brackets lookup
    static constexpr std::array<std::pair<char, char>, 4> brackets{
    {{'{', '}'},
     {'(', ')'},
     {'<', '>'},
     {'[', ']'}}};

    constexpr auto lookup = []( const char c ){ return std::find_if(begin(brackets), end(brackets), [&c](const auto &v) { return v.first == c; })->second;};

    constexpr auto is_char = []( const char c ){ return c >= 'A' && c <= 'Z';};
    constexpr auto is_open_br = []( const char c ){ return c == '(' || c == '[' || c == '<' || c == '{';};
}

namespace yafth
{
    engine::engine(const LockLevel lockLevelSetting_, const uint32_t playerScienceSkill_, const uint64_t seed_ = 0) 
        : rng(0, random::seed(seed_))
        , lockLevelSetting(lockLevelSetting_)
        , playerScienceSkill(std::clamp<uint32_t>(playerScienceSkill_, 1, 100))
        , wordLength(set_word_length())
        , wordCount(set_word_count())
        , answer(rng.next() % wordCount)
    {
        generate_term_chars();
        generate_words();
        place_words();
    }

    inline std::size_t engine::set_word_length() noexcept
    {
        return std::min<std::size_t>(12, 4 + 2 * static_cast<std::size_t>(lockLevelSetting) + (rng.next() & 1));
    }

    // from https://geckwiki.com/index.php?title=Hacking_Word_Count
    std::size_t engine::set_word_count() noexcept
    {
        constexpr std::size_t iHackingMinWords = 5;
        constexpr std::size_t iHackingMaxWords = 20;
        const std::size_t lockLevel = (static_cast<std::size_t>(lockLevelSetting) * 0.25) * 100;
        const std::size_t scienceOffset = playerScienceSkill - lockLevel;
        const std::size_t lockOffset = 100 - lockLevel;

        const double sol_coef = lockOffset == 0 ? 0.5 : (1 - ( (1.0 * scienceOffset) / lockOffset) ); // scienceOffset over lockOffset coef

        const std::size_t wordCount = (sol_coef * (iHackingMaxWords - iHackingMinWords)) + iHackingMinWords;
        
        return std::min<std::size_t>(20, wordCount);
    }

    void engine::generate_term_chars() noexcept
    {
        constexpr std::array<char, 24> placeholders = 
            {'.', ',', '!', '?', '/', '*', '+', '\'', ':', ';', '-', '_', '%', '$', '|', '@', '{', '}', '[', ']', '(', ')', '<', '>'};
        for (auto & c : term_chars){
            c = placeholders[rng.next() % 24];
        }
    }

    void engine::generate_words() noexcept
    {
        std::array<std::string, 100> wordArr{};
        if (wordLength == 4)
        {
            wordArr  = { "THAT", "WITH", "FROM", "WERE", "THIS", "THEY", "HAVE", "SAID", "WHAT", "WHEN", "BEEN", "THEM", "INTO", "MORE", "ONLY", "WILL", "THEN", "SOME", "TIME", "SUCH", "VERY", "OVER", "YOUR", "THAN", "WELL", "DOWN", "FACE", "UPON", "LIKE", "SAME", "KNOW", "WENT", "MADE", "LONG", "CAME", "ROOM", "MUST", "EVEN", "EYES", "COME", "MOST", "LIFE", "AWAY", "HAND", "LEFT", "ALSO", "JUST", "ARMY", "BACK", "GOOD", "HEAD", "MARY", "PART", "FELT", "HERE", "MUCH", "BONE", "TAKE", "MANY", "SKIN", "TOOK", "ONCE", "LOOK", "LAST", "BOTH", "GIVE", "SIDE", "FORM", "MAKE", "DOOR", "KNEW", "TELL", "TOLD", "WHOM", "LOVE", "DAYS", "DEAR", "SEEN", "GAVE", "CASE", "DONE", "FREE", "SOON", "EACH", "WORK", "LESS", "WIFE", "DOES", "MIND", "BODY", "OPEN", "WANT", "SENT", "HALF", "YEAR", "PAIN", "WORD", "HOME", "ANNA", "FIND" };
        }
        if (wordLength == 5)
        {
            wordArr  = { "WHICH", "THERE", "THEIR", "WOULD", "COULD", "AFTER", "OTHER", "ABOUT", "THESE", "THOSE", "FIRST", "WHERE", "UNDER", "STILL", "BEING", "AGAIN", "BEGAN", "GREAT", "ASKED", "WHILE", "COUNT", "WHOLE", "SHALL", "RIGHT", "PLACE", "STATE", "HOUSE", "EVERY", "HEARD", "YOUNG", "NEVER", "THREE", "YEARS", "THINK", "ROUND", "FOUND", "BLOOD", "POWER", "MIGHT", "SMALL", "QUITE", "LARGE", "VOICE", "WORDS", "HANDS", "CASES", "AMONG", "SONYA", "OFTEN", "TAKEN", "SMILE", "KNOWN", "ALONG", "ORDER", "WOMEN", "NIGHT", "STOOD", "JOINT", "CAUSE", "GOING", "GIVEN", "WORLD", "CHIEF", "FRONT", "WHITE", "ALONE", "HORSE", "LATER", "DEATH", "LABOR", "MONEY", "UNTIL", "WOMAN", "NERVE", "SOUTH", "WOUND", "THING", "LEAVE", "ABOVE", "ADDED", "PARTY", "PARTS", "TABLE", "BORIS", "ENEMY", "CRIED", "LIGHT", "UNION", "EARLY", "PETYA", "BONES", "SINCE", "HEART", "FORMS", "SPEAK", "MEANS", "MOVED", "FORCE", "THIRD", "SHORT" };
        }
        if (wordLength == 6)
        {
            wordArr  = { "PIERRE", "PRINCE", "BEFORE", "SHOULD", "ANDREW", "FRENCH", "LITTLE", "STATES", "PEOPLE", "ROSTOV", "LOOKED", "MOSCOW", "SEEMED", "HAVING", "THOUGH", "ALWAYS", "TISSUE", "UNITED", "FATHER", "DURING", "TURNED", "MOMENT", "HOLMES", "CALLED", "BATTLE", "OTHERS", "BECOME", "BEHIND", "COURSE", "RESULT", "PASSED", "MATTER", "ACTION", "TOWARD", "ALMOST", "THINGS", "TROOPS", "BECAME", "WITHIN", "MOTHER", "TAKING", "NUMBER", "EITHER", "LETTER", "PUBLIC", "COMMON", "FRIEND", "SECOND", "CANNOT", "GLANDS", "ITSELF", "REALLY", "TWENTY", "AROUND", "SAYING", "BETTER", "HORSES", "VASILI", "SYSTEM", "FORMED", "FELLOW", "LONGER", "MYSELF", "BEYOND", "GROWTH", "ANYONE", "ORDERS", "TUMOUR", "ACROSS", "RATHER", "BESIDE", "COMING", "MAKING", "OPENED", "WANTED", "RAISED", "FAMILY", "WISHED", "SEEING", "ANSWER", "EVENTS", "PERIOD", "GERMAN", "RUSSIA", "SPREAD", "MIDDLE", "REASON", "RETURN", "MERELY", "SILENT", "EFFECT", "GIVING", "WINDOW", "WOUNDS", "PERSON", "DINNER", "DOCTOR", "PLACED", "TRYING", "STREET" };
        }
        if (wordLength == 7)
        {
            wordArr  = { "NATASHA", "HIMSELF", "WITHOUT", "THOUGHT", "ANOTHER", "GENERAL", "THROUGH", "AGAINST", "BETWEEN", "NOTHING", "EMPEROR", "BECAUSE", "DISEASE", "KUTUZOV", "USUALLY", "LOOKING", "ALREADY", "CHAPTER", "OFFICER", "RUSSIAN", "HISTORY", "DENISOV", "HOWEVER", "COUNTRY", "BROUGHT", "PATIENT", "SEVERAL", "FEELING", "CERTAIN", "WHETHER", "HERSELF", "MORNING", "PRESENT", "REPLIED", "ENGLAND", "AMERICA", "TISSUES", "PROJECT", "EXAMPLE", "CARRIED", "ENTERED", "SURFACE", "SITTING", "BRITISH", "EVENING", "SHOUTED", "VESSELS", "BECOMES", "DRAWING", "FORWARD", "HUNDRED", "RESULTS", "WOUNDED", "SERVICE", "ANATOLE", "REACHED", "STRANGE", "PROCESS", "OPINION", "SOLDIER", "AFFAIRS", "ENGLISH", "SLAVERY", "STOPPED", "PERHAPS", "TURNING", "APPLIED", "MUSCLES", "TALKING", "RAPIDLY", "HUSBAND", "ABSCESS", "COMPANY", "NOTICED", "TUMOURS", "FEDERAL", "SUBJECT", "BELIEVE", "WAITING", "QUICKLY", "REMOVED", "ACCOUNT", "BROTHER", "FOREIGN", "GLANCED", "FREEDOM", "COVERED", "SOCIETY", "CHANGES", "NEITHER", "SMILING", "ROSTOVS", "SOMEONE", "VILLAGE", "LESIONS", "SPECIAL", "MEMBERS", "FINALLY", "FRIENDS", "VARIOUS" };
        }
        if (wordLength == 8)
        {
            wordArr  = { "PRINCESS", "AMERICAN", "NICHOLAS", "NAPOLEON", "COUNTESS", "SUDDENLY", "CONGRESS", "POSITION", "SOLDIERS", "ANYTHING", "QUESTION", "MOVEMENT", "POSSIBLE", "DOLOKHOV", "FOLLOWED", "BUSINESS", "OFFICERS", "NATIONAL", "RECEIVED", "TOGETHER", "THOUSAND", "EVERYONE", "PRESSURE", "CHILDREN", "REMAINED", "REGIMENT", "MILITARY", "ANSWERED", "INTEREST", "STANDING", "PRESENCE", "ANEURYSM", "REPEATED", "HAPPENED", "COLONIES", "AFFECTED", "APPEARED", "SOUTHERN", "TERRIBLE", "RETURNED", "FEATURES", "CAMPAIGN", "SPEAKING", "DAUGHTER", "STRENGTH", "COLONIAL", "MEASURES", "SYMPTOMS", "SYPHILIS", "GANGRENE", "REMARKED", "ALTHOUGH", "SWELLING", "ADJUTANT", "YOURSELF", "REMEMBER", "CLINICAL", "CONTRARY", "EMPLOYED", "LISTENED", "MEMBRANE", "PAVLOVNA", "RUSSIANS", "VIRGINIA", "PROBABLY", "COMPLETE", "GOVERNOR", "CARRIAGE", "DECLARED", "CONSISTS", "PLEASURE", "RELATION", "PREPARED", "ALPATYCH", "PROPERTY", "THINKING", "DISEASES", "INDUSTRY", "ATTENDED", "PRODUCED", "ACTIVITY", "OBSERVED", "DRESSING", "PEASANTS", "EXPECTED", "THOUGHTS", "STRAIGHT", "MAJORITY", "OCCURRED", "ECONOMIC", "COMMERCE", "ELECTION", "DISTANCE", "GENERALS", "HANDSOME", "OCCUPIED", "POLITICS", "SHOULDER", "BACTERIA", "INCREASE" };
        }
        if (wordLength == 9)
        {
            wordArr  = { "SOMETHING", "TREATMENT", "SOMETIMES", "EVIDENTLY", "PRESIDENT", "INFECTION", "CONDITION", "NECESSARY", "COMMANDER", "CONTINUED", "IMPORTANT", "DIFFERENT", "GUTENBERG", "POLITICAL", "FORMATION", "OPERATION", "FOLLOWING", "ATTENTION", "THEREFORE", "QUESTIONS", "BOLKONSKI", "CHARACTER", "ACCORDING", "BAGRATION", "ALEXANDER", "DESCRIBED", "RELATIONS", "DIFFICULT", "SECONDARY", "DIRECTION", "BEGINNING", "HAPPINESS", "INCREASED", "INFLUENCE", "EXPRESSED", "BOURIENNE", "DESTROYED", "EVERYBODY", "JEFFERSON", "ORGANISMS", "SHOULDERS", "RESULTING", "PRISONERS", "CERTAINLY", "PRESENTED", "ARTICULAR", "EXCLAIMED", "INTERESTS", "MOVEMENTS", "DIAGNOSIS", "TERRITORY", "SUFFERING", "VARIETIES", "CARTILAGE", "FRENCHMAN", "AUTHORITY", "DISCHARGE", "GENTLEMEN", "GENTLEMAN", "BEAUTIFUL", "DEMOCRACY", "WHISPERED", "BONAPARTE", "DEMOCRATS", "THOUSANDS", "DANGEROUS", "ESSENTIAL", "APPOINTED", "GENERALLY", "ROOSEVELT", "AMENDMENT", "GRADUALLY", "LISTENING", "INDICATED", "MALIGNANT", "SITUATION", "NECESSITY", "BRILLIANT", "INCLUDING", "AGITATION", "CONVINCED", "SOVEREIGN", "COMPANION", "CONCERNED", "SURPRISED", "AMERICANS", "SPERANSKI", "BENNIGSEN", "OURSELVES", "PROMINENT", "PARALYSIS", "SEPARATED", "HURRIEDLY", "AFFECTION", "COMPANIES", "SENSATION", "ABANDONED", "ADDRESSED", "CAREFULLY", "ARTHRITIS" };
        }
        if (wordLength == 10)
        {
            wordArr  = { "GOVERNMENT", "EVERYTHING", "UNDERSTAND", "ESPECIALLY", "EXPRESSION", "THEMSELVES", "IMPOSSIBLE", "PETERSBURG", "CONDITIONS", "UNDERSTOOD", "FREQUENTLY", "REVOLUTION", "ASSOCIATED", "REPUBLICAN", "WASHINGTON", "CONSIDERED", "CONVENTION", "SYPHILITIC", "FRIGHTENED", "APPEARANCE", "HISTORIANS", "EXCELLENCY", "ROSTOPCHIN", "REMEMBERED", "INDIVIDUAL", "IMPORTANCE", "DIFFICULTY", "EXPERIENCE", "DMITRIEVNA", "SURROUNDED", "CONNECTION", "CONNECTIVE", "POPULATION", "INDUSTRIAL", "OPPOSITION", "TENDERNESS", "COMPLETELY", "PARTICULAR", "AFFECTIONS", "FOUNDATION", "RECOGNIZED", "PRINCIPLES", "APPROACHED", "CONCEPTION", "DEMOCRATIC", "INTRODUCED", "OPERATIONS", "THROUGHOUT", "REMARKABLE", "PERIOSTEUM", "SUFFICIENT", "ADDRESSING", "AMPUTATION", "AFTERWARDS", "ENTERPRISE", "PROPORTION", "SETTLEMENT", "ABSOLUTELY", "COMPROMISE", "CHARACTERS", "INCREASING", "APPARENTLY", "COMMERCIAL", "CONTAINING", "IMPRESSION", "OCCURRENCE", "ACCUSTOMED", "HISTORICAL", "INTERESTED", "PARLIAMENT", "COMMANDERS", "DETERMINED", "INDUSTRIES", "IRRITATION", "PROTECTION", "RECOGNISED", "COMMISSION", "EXCITEMENT", "INEVITABLE", "EVERYWHERE", "CONCLUSION", "ELECTRONIC", "LEADERSHIP", "LEUCOCYTES", "RESOLUTION", "SUCCESSFUL", "CALIFORNIA", "EPITHELIUM", "POSSESSION", "PREVIOUSLY", "DIPLOMATIC", "CONFIDENCE", "PROTECTIVE", "ULCERATION", "UNPLEASANT", "ASSISTANCE", "HYPERAEMIA", "OCCUPATION", "PERMISSION", "PROVISIONS" };
        }
        if (wordLength == 11)
        {
            wordArr  = { "TUBERCULOUS", "IMMEDIATELY", "HAEMORRHAGE", "SUPPURATION", "REPUBLICANS", "MIKHAYLOVNA", "ESTABLISHED", "DEVELOPMENT", "DESTRUCTION", "INTERRUPTED", "TEMPERATURE", "EXPERIENCED", "SURROUNDING", "SUPERFICIAL", "CIRCULATION", "CONTINUALLY", "POSSIBILITY", "ACCOMPANIED", "GRANULATION", "INHABITANTS", "LEGISLATURE", "MISSISSIPPI", "APPLICATION", "INDEPENDENT", "INFORMATION", "EXAMINATION", "INTERESTING", "APPROACHING", "IMMIGRATION", "OPPORTUNITY", "CONTRACTION", "LEGISLATION", "DISAPPEARED", "EXPLANATION", "AGRICULTURE", "AUTHORITIES", "ENLARGEMENT", "CONSIDERING", "FEDERALISTS", "PROGRESSIVE", "TRANSFERRED", "CONNECTICUT", "DECLARATION", "IMAGINATION", "BOGUCHAROVO", "EPITHELIOMA", "CONTINENTAL", "ASSOCIATION", "COMBINATION", "TERRITORIES", "DISTURBANCE", "REPRESENTED", "CICATRICIAL", "DISTRIBUTED", "SENSIBILITY", "APPOINTMENT", "COMPRESSION", "OBSERVATION", "SIGNIFICANT", "DISLOCATION", "INNUMERABLE", "BATTLEFIELD", "DISPOSITION", "OBLIGATIONS", "EXCEPTIONAL", "RESTORATION", "UNFORTUNATE", "ARRANGEMENT", "RECOGNIZING", "CONFEDERATE", "CORPORATION", "DISTINGUISH", "GOVERNMENTS", "CONTROVERSY", "INQUIRINGLY", "PREPARATION", "RECOGNITION", "APPROPRIATE", "ATTENTIVELY", "EXCEEDINGLY", "IMPROVEMENT", "INDIVIDUALS", "PRACTICALLY", "RESPONSIBLE", "APPEARANCES", "INDIFFERENT", "LEGISLATIVE", "NECESSARILY", "RECOMMENDED", "SUPPURATIVE", "UNDOUBTEDLY", "CONFEDERACY", "CONNECTIONS", "UNNECESSARY", "COMPLICATED", "CONVENTIONS", "PHILIPPINES", "CONSEQUENCE", "CONSTITUTES", "CONTRACTURE" };
        }
        if (wordLength == 12)
        {
            wordArr  = { "CONSTITUTION", "ILLUSTRATION", "CONVERSATION", "PARTICULARLY", "CONSIDERABLE", "INDEPENDENCE", "MADEMOISELLE", "SUBCUTANEOUS", "PENNSYLVANIA", "INFLAMMATION", "OCCASIONALLY", "SIGNIFICANCE", "INTERFERENCE", "TUBERCULOSIS", "LEGISLATURES", "SATISFACTION", "PHILADELPHIA", "DEGENERATION", "DISTRIBUTION", "GRANULATIONS", "UNEXPECTEDLY", "ACQUAINTANCE", "HANDKERCHIEF", "DIFFICULTIES", "INFLAMMATORY", "HEADQUARTERS", "INSTRUCTIONS", "PROCLAMATION", "CONSERVATIVE", "PREPARATIONS", "MANUFACTURES", "SURROUNDINGS", "ORGANIZATION", "RESPECTFULLY", "SATISFACTORY", "CIVILIZATION", "INTELLECTUAL", "ACCOMPLISHED", "LEUCOCYTOSIS", "NEVERTHELESS", "RATIFICATION", "REGENERATION", "OSSIFICATION", "PATHOLOGICAL", "DISPOSITIONS", "DISSATISFIED", "INSTITUTIONS", "COMBINATIONS", "NEGOTIATIONS", "PRESIDENTIAL", "SUFFICIENTLY", "CONCENTRATED", "INTRODUCTION", "ARRANGEMENTS", "CONSEQUENTLY", "ASTONISHMENT", "PROFESSIONAL", "IMPROVEMENTS", "CORPORATIONS", "DISPLACEMENT", "INDIFFERENCE", "SUBSEQUENTLY", "ACCUMULATION", "CONSTRUCTION", "INTERVENTION", "SUCCESSFULLY", "FIBROMATOSIS", "INFILTRATION", "VERESHCHAGIN", "AFFECTIONATE", "APPLICATIONS", "DELIBERATELY", "EMANCIPATION", "LYMPHANGITIS", "PASSIONATELY", "RECOLLECTION", "REPRESENTING", "RESTRICTIONS", "AGRICULTURAL", "ESTABLISHING", "IRRESISTIBLE", "ANNOUNCEMENT", "CONGRATULATE", "DEMONSTRATED", "DIFFERENTIAL", "HENDRIKHOVNA", "REQUIREMENTS", "ACCIDENTALLY", "MILORADOVICH", "COMMENCEMENT", "COMPENSATION", "DISTRIBUTING", "STRENGTHENED", "TRANQUILLITY", "ACCOMPANYING", "ADMINISTERED", "ADVANTAGEOUS", "COLONIZATION", "CONSEQUENCES", "CONSIDERABLY" };
        }
        uint32_t i = 0;
        auto it = words_chars.begin();
        do
        {
            auto nextWord = wordArr[rng.next() % 100];
            if(std::find(words.begin(), words.end(), nextWord) == words.end())
            {
                std::copy(nextWord.begin(), nextWord.end(), it);
                words[i] = std::string_view{it, it + wordLength};
                it += wordLength;
                ++i;
            }
        } while (i < wordCount);
    }

    void engine::place_words() noexcept
    {
        const uint32_t spacePerWord = term_chars.size() / wordCount;
        const uint32_t possibleStart = spacePerWord - wordLength;
        for(uint32_t id = 0; id < wordCount; ++id)
        {
            auto iter = term_chars.begin() + id * spacePerWord + rng.next() % possibleStart;
            std::copy(words[id].begin(), words[id].end(), iter);
        }
    }

    // std::string engine::print_formatted() const
    // {
    //     constexpr uint32_t columns = 2;
    //     constexpr uint32_t rows = 17;
    //     constexpr uint32_t row_length = 12;

    //     std::stringstream ss;

    //     for(uint32_t i = 0; i < columns * rows; ++ i)
    //     {
    //         const uint32_t start = (i % 2) * (rows * row_length) + (i / 2) * 12;
    //         ss << ( i == 0 ? "  " : ( start < 100 ? " " : "" ) ) << ( start ) << ' ';
    //         for(auto j = start; j < start + row_length; ++j) ss << term_chars[j];
    //         ss << ( (i % 2) ? '\n' : ' ' );
    //     }
    //     return ss.str();
    // }

    std::string_view engine::look_at(std::size_t i) const
    {
        i = std::clamp<std::size_t>(i, 0, term_chars.size());
        if ( ::is_char(term_chars[i]) ) // case of word
        {
            auto l = term_chars.begin() + i;
            auto r = l;
            while(l != term_chars.begin() && ::is_char(*l)) --l; 
            if ( !::is_char(*l) ) ++l;
            while (r != term_chars.end() && ::is_char(*r) ) ++r;
            return {l, r};
        }
        else if ( ::is_open_br(term_chars[i]) ) // case of brackets
        {
            const std::size_t j =  ( (i / 12) + 1 ) * 12;
            const char c = ::lookup(term_chars[i]);
            auto e = term_chars.begin() + j;
            for(auto it = term_chars.begin() + i; it != e; ++it)
            {
                if( ::is_char(*it) )
                {
                    e = term_chars.begin() + i;
                    break;
                }
                if(*it == c)
                {
                    e = it;
                    break;
                }
            }
            if(e != term_chars.begin() + j)
            {
                return {term_chars.begin() + i, e + 1};
            }
        }
        //case of one symbol
        return {term_chars.begin() + i, term_chars.begin() + i + 1};
    }

    // void engine::click_at(std::size_t i)
    // {
    //     auto [b, e] = look_at(i);
    //     if(b == e + 1)
    //     { // nothing interesing

    //     } 
    //     else // brackets or word
    //     {

    //     }
    // }
}