// Copyright 2016 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_INSPECTOR_WASM_TRANSLATION_H_
#define V8_INSPECTOR_WASM_TRANSLATION_H_

#include <unordered_map>

#include "include/v8.h"
#include "src/base/macros.h"
#include "src/debug/debug-interface.h"
#include "src/inspector/string-16.h"

namespace v8_inspector {

// Forward declarations.
class V8DebuggerAgentImpl;

class WasmTranslation {
    public:
        enum Mode { Raw, Disassemble };

        explicit WasmTranslation(v8::Isolate* isolate);
        ~WasmTranslation();

        // Set translation mode.
        void SetMode(Mode mode) {
            mode_ = mode;
        }

        // Make a wasm script known to the translation. This will trigger a number of
        // didParseScript calls to the given debugger agent.
        // Only locations referencing a registered script will be translated by the
        // Translate functions below.
        void AddScript(v8::Local<v8::debug::WasmScript> script,
                       V8DebuggerAgentImpl* agent);

        // Clear all registered scripts.
        void Clear();

        // Translate a location as generated by V8 to a location that should be sent
        // over protocol.
        // Does nothing for locations referencing a script which was not registered
        // before via AddScript.
        // Line and column are 0-based.
        // Returns true if the location was translated, false otherwise.
        bool TranslateWasmScriptLocationToProtocolLocation(String16* script_id,
                int* line_number,
                int* column_number);

        // Translate back from protocol locations (potentially referencing artificial
        // scripts for individual wasm functions) to locations that make sense to V8.
        // Does nothing if the location was not generated by the translate method
        // above.
        // Returns true if the location was translated, false otherwise.
        bool TranslateProtocolLocationToWasmScriptLocation(String16* script_id,
                int* line_number,
                int* column_number);

        const String16& GetSource(const String16& script_id, int func_index);
        int GetStartLine(const String16& script_id, int func_index) {
            return 0;
        }
        int GetStartColumn(const String16& script_id, int func_index) {
            return 0;
        }
        int GetEndLine(const String16& script_id, int func_index);
        int GetEndColumn(const String16& script_id, int func_index);
        String16 GetHash(const String16& script_id, int func_index);

    private:
        class TranslatorImpl;
        friend class TranslatorImpl;

        void AddFakeScript(const String16& scriptId, TranslatorImpl* translator);

        v8::Isolate* isolate_;
        std::unordered_map<int, std::unique_ptr<TranslatorImpl>> wasm_translators_;
        std::unordered_map<String16, TranslatorImpl*> fake_scripts_;
        Mode mode_;

        DISALLOW_COPY_AND_ASSIGN(WasmTranslation);
};

}  // namespace v8_inspector

#endif  // V8_INSPECTOR_WASM_TRANSLATION_H_
