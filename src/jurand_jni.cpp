#include <org_fedoraproject_javapackages_Jurand.h>
#include <java_symbols.hpp>

static jclass illegal_argument_exception_class(JNIEnv* env)
{
	return env->FindClass("Ljava/lang/IllegalArgumentException;");
}

static jclass runtime_exception_class(JNIEnv* env)
{
	return env->FindClass("Ljava/lang/RuntimeException;");
}

extern "C" void Java_org_fedoraproject_javapackages_Jurand_execute(
	JNIEnv* env,
	[[maybe_unused]] jclass self,
	jobjectArray fileroots,
	jobjectArray names,
	jobjectArray patterns,
	jboolean strict_mode,
	jboolean remove_annotations)
{
	auto path_to_string_id = env->GetMethodID(env->FindClass("Ljava/nio/file/Path;"), "toString", "()Ljava/lang/String;");
	
	auto parameters = Parameters();
	
	auto fileroots_len = env->GetArrayLength(fileroots);
	auto fileroots_vec = std::vector<std::string_view>(fileroots_len);
	for (jsize i = 0; i != fileroots_len; ++i)
	{
		auto path = env->GetObjectArrayElement(fileroots, i);
		auto path_string = static_cast<jstring>(env->CallObjectMethod(path, path_to_string_id));
		auto path_chars = env->GetStringUTFChars(path_string, nullptr);
		auto path_chars_len = env->GetStringUTFLength(path_string);
		fileroots_vec[i] = std::string_view(path_chars, path_chars_len);
		// NOTE by not releasing the chars we ensure the lifetime of the string_view
		// env->ReleaseStringUTFChars(path_string, path_chars);
	}
	
	auto names_len = env->GetArrayLength(names);
	for (jsize i = 0; i != names_len; ++i)
	{
		auto name = static_cast<jstring>(env->GetObjectArrayElement(names, i));
		auto name_chars = env->GetStringUTFChars(name, nullptr);
		auto name_chars_len = env->GetStringUTFLength(name);
		parameters.names_.emplace(name_chars, name_chars_len);
		// NOTE by not releasing the chars we ensure the lifetime of the string_view
		// env->ReleaseStringUTFChars(name, name_chars);
	}
	
	auto patterns_len = env->GetArrayLength(patterns);
	parameters.patterns_.reserve(patterns_len);
	for (jsize i = 0; i != patterns_len; ++i)
	{
		auto pattern = static_cast<jstring>(env->GetObjectArrayElement(patterns, i));
		auto pattern_chars = env->GetStringUTFChars(pattern, nullptr);
		auto pattern_chars_len = env->GetStringUTFLength(pattern);
		parameters.patterns_.emplace_back(std::string_view(pattern_chars, pattern_chars_len), std::regex_constants::extended);
		env->ReleaseStringUTFChars(pattern, pattern_chars);
	}
	
	parameters.in_place_ = true;
	parameters.also_remove_annotations_ = remove_annotations;
	parameters.strict_mode_ = strict_mode;
	
	// Do not accept empty input files, we do not read from standard input from JNI
	if (fileroots_vec.empty())
	{
		env->ThrowNew(illegal_argument_exception_class(env), "jurand: no input files");
	}
	
	auto [exit_code, error_message] = java_symbols::execute(fileroots_vec, parameters);
	
	if (exit_code == 0)
	{
		return;
	}
	else if (exit_code == 1)
	{
		env->ThrowNew(illegal_argument_exception_class(env), error_message.c_str());
	}
	else
	{
		env->ThrowNew(runtime_exception_class(env), error_message.c_str());
	}
}
