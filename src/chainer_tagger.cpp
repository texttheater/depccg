
#include <Python.h>
#include "py/tagger.h"
#include "chainer_tagger.h"

namespace myccg {

char PYPATH[] = "PYTHONPATH=.";

// std::unique_ptr<float[]> ChainerTagger::predict(const std::string& tokens) const {
//     putenv(PYPATH);
//     if ( !Py_IsInitialized() )
//         Py_Initialize();
//     initchainer_tagger();
//     std::unique_ptr<float[]> res(
//             new float[ this->TargetSize() * tokens.size() ]);
//     tag(this->model_.c_str(), tokens.c_str(), tokens.size(), res.get());
//     return res;
// }
//

Probs ChainerTagger::PredictTags(Doc& doc) const {
    putenv(PYPATH);
    if ( !Py_IsInitialized() )
        Py_Initialize();
    initchainer_tagger();
    float** res = new float*[doc.size()];
    const char** c_sents = new const char*[doc.size()];
    int* lengths = new int[doc.size()];
    for (unsigned i = 0; i < doc.size(); i++) {
        auto& sent = doc[i];
        c_sents[i] = sent.c_str();
        lengths[i] = (int)sent.size();
        res[i] = new float[this->TargetSize() * sent.size()];
    }
    tag_doc(this->model_.c_str(), c_sents, lengths, doc.size(), res);
    delete[] c_sents;
    delete[] lengths;
    return std::unique_ptr<float*[]>(res);
}

std::pair<Probs, Probs> ChainerDependencyTagger::PredictTagsAndDeps(Doc& doc) const {
    putenv(PYPATH);
    if ( !Py_IsInitialized() )
        Py_Initialize();
    initchainer_tagger();
    float** cats = new float*[doc.size()];
    float** deps = new float*[doc.size()];
    const char** c_sents = new const char*[doc.size()];
    int* lengths = new int[doc.size()];
    for (unsigned i = 0; i < doc.size(); i++) {
        auto& sent = doc[i];
        c_sents[i] = sent.c_str();
        lengths[i] = (int)sent.size();
        cats[i] = new float[this->TargetSize() * sent.size()];
        deps[i] = new float[sent.size() * sent.size()];
    }
    tag_and_parse_doc(this->model_.c_str(),
            c_sents, lengths, doc.size(), cats, deps);
    delete[] c_sents;
    delete[] lengths;
    return std::make_pair(
            std::unique_ptr<float*[]>(cats), std::unique_ptr<float*[]>(deps));
}

} // namespace myccg

