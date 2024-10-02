// yafth
#include "yafth/random.h"
#include "yafth/engine.h"
// stl
#include <algorithm>
#include <numeric>

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
        , wordsLeft(wordCount)
    {
        generate_term_chars();
        generate_words();
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
        const std::size_t lockLevel = static_cast<std::size_t>(lockLevelSetting) * 25;
        const std::size_t scienceOffset = playerScienceSkill - lockLevel;
        const std::size_t lockOffset = 100 - lockLevel;

        const double sol_coef = lockOffset == 0 ? 0.5 : (1 - ( (1.0 * scienceOffset) / lockOffset) ); // scienceOffset over lockOffset coef

        const std::size_t wordCount_ = static_cast<std::size_t>(sol_coef * (iHackingMaxWords - iHackingMinWords)) + iHackingMinWords;
        
        return std::min<std::size_t>(20, wordCount_);
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
        std::size_t i = 0;
        std::array<char, 12 * 20> words_chars;
        auto it = words_chars.begin();
        std::array<std::string_view, 20> words_tmp;
        do
        {
            auto nextWord = wordArr[rng.next() % 100];
            if(std::find(words_tmp.begin(), words_tmp.end(), nextWord) == words_tmp.end())
            {
                std::copy(nextWord.begin(), nextWord.end(), it);
                words_tmp[i] = std::string_view{it, it + wordLength};
                it += wordLength;
                ++i;
            }
        } while (i < wordCount);

        const std::size_t spacePerWord = term_chars.size() / wordCount;
        const std::size_t possibleStart = spacePerWord - wordLength;
        for(std::size_t id = 0; id < wordCount; ++id)
        {
            auto iter = term_chars.begin() + id * spacePerWord + rng.next() % possibleStart;
            std::copy(words_tmp[id].begin(), words_tmp[id].end(), iter);
            words[id] = std::distance(term_chars.begin(), iter);
        }

        std::fill(words_chars.begin(), words_chars.end(), '*');
    }

    std::pair<std::size_t, std::size_t> engine::look_at(std::size_t i) const
    {
        i = std::clamp<std::size_t>(i, 0, term_chars.size() - 1);
        if ( ::is_char(term_chars[i]) ) // case of word
        {
            auto l = i;
            auto r = l;
            while(l != 0 && ::is_char(term_chars[l])) --l; 
            if ( !::is_char(term_chars[l]) ) ++l;
            while (r != term_chars.size() && ::is_char(term_chars[r]) ) ++r;
            return {l, r};
        }
        else if ( ::is_open_br(term_chars[i]) && !used_bracket.test(i)) // case of brackets
        {
            const std::size_t j =  ( (i / 12) + 1 ) * 12;
            const char c = ::lookup(term_chars[i]);
            auto e = j;
            for(auto it = i; it != e; ++it)
            {
                if( ::is_char(term_chars[it]) )
                {
                    e = i;
                    break;
                }
                if(term_chars[it] == c)
                {
                    e = it;
                    break;
                }
            }
            if(e != j)
            {
                return {i, e + 1};
            }
        }
        //case of one symbol
        return {i, i + 1};
    }

    // this code is shit
    click_status engine::click_at(std::size_t i)
    {
        i = std::clamp<std::size_t>(i, 0, term_chars.size() - 1);
        const auto &[b, e] = look_at(i);
        std::string_view substr{term_chars.begin() + b, term_chars.begin() + e};
        const std::string_view term{term_chars.begin(), term_chars.end()};
        if(e - b != 1)
        {
            if(::is_char(term[b])) // word
            {
                if( term.substr(words[answer], wordLength) == substr)
                {
                    return {ClickState::ExactMatch, {}};
                }
                else // it's not an answer
                {
                    --attemptsLeft;
                    const std::size_t match = std::inner_product(substr.begin(), substr.end(), term.begin() + words[answer], 0, std::plus<>(), std::equal_to<>());
                    const std::size_t offset = term.find(substr);
                    std::fill(term_chars.begin() + offset, term_chars.begin() + offset + wordLength, '.');
                    std::iter_swap(words.begin() + wordsLeft - 1, std::find(words.begin(), words.end(), offset));
                    --wordsLeft;
                    return {ClickState::EntryDenied, match};
                }
            }
            else // bracket
            {
                const std::size_t dist = std::distance<const char *>(term_chars.data(), substr.data()); // seem like a problem, but works fine
                if( !used_bracket.test( dist ) )
                {
                    used_bracket.set( dist );

                    if(wordsLeft == 1)
                    {
                        attemptsLeft = 4;
                        return {ClickState::AllowanceReplenished, {}};
                    }
                    else // wordsLeft != 1
                    {
                        if(attemptsLeft == 4)
                        {
                            // remove dud
                            std::size_t dud_id = 1 + ( rng.next() % (wordsLeft - 1 ) );
                            const std::size_t offset = words[dud_id];
                            std::fill(term_chars.begin() + offset, term_chars.begin() + offset + wordLength, '.');
                            std::iter_swap(words.begin() + wordsLeft - 1, words.begin() + dud_id);
                            --wordsLeft;
                            
                            return {ClickState::DudRemoved, {}};
                        }
                        else // attemptsLeft != 4 
                        {
                            if( (rng.next() & 1) == 1)
                            {
                                // remove dud
                                std::size_t dud_id = 1 + ( rng.next() % (wordsLeft - 1 ) );
                                const std::size_t offset = words[dud_id];
                                std::fill(term_chars.begin() + offset, term_chars.begin() + offset + wordLength, '.');
                                std::iter_swap(words.begin() + wordsLeft - 1, words.begin() + dud_id);
                                --wordsLeft;
                                
                                return {ClickState::DudRemoved, {}};
                            }
                            else
                            {
                                attemptsLeft = 4;
                                return {ClickState::AllowanceReplenished, {}};
                            }
                        }
                    }
                } 
            }   
        } 
        return {ClickState::Error, {}};
    }
}